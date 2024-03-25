#include "opencvimage.hpp"

#include "logging.hpp"

const std::string tempDirPath {"temp"};

void Analyse::ImageObject::setupVariations(std::vector<cv::Mat> &templateImages)
{
    size_t currentIndex = templateImages.size() - 1;

    // Normal rotations
    currentIndex = setupRotations(templateImages);

    // Mirror vertically
    templateImages.push_back({});
    cv::flip(templateImages[0], templateImages[currentIndex], 0);

    // Mirrored vertical
    currentIndex = setupRotations(templateImages);

    // Mirror horizontally
    templateImages.push_back({});
    cv::flip(templateImages[0], templateImages[currentIndex], 1);

    // Mirrored horizontal
    currentIndex = setupRotations(templateImages);

    return currentIndex;
}

size_t Analyse::ImageObject::setupRotations(std::vector<cv::Mat> &templateImages)
{
    currentIndex = templateImages.size() - 1;

    // Rotate 90
    templateImages.push_back({});
    cv::transpose(templateImages[currentIndex], templateImages[currentIndex + 1]);
    cv::flip(templateImages[currentIndex], templateImages[currentIndex], 1);

    // Rotate -90
    templateImages.push_back({});
    cv::transpose(templateImages[currentIndex], templateImages[currentIndex + 2]);
    cv::flip(templateImages[currentIndex], templateImages[currentIndex], 0);

    // Rotate 180
    templateImages.push_back({});
    cv::flip(templateImages[currentIndex], templateImages[currentIndex + 3], -1);

    return currentIndex + 4;
}

cv::Mat Analyse::ImageObject::cropToContour(cv::Mat& image)
{
    cv::Mat greyCopy;
    cv::cvtColor(image, greyCopy, cv::COLOR_BGR2GRAY);
    if (greyCopy.empty())
    {
        LOG_WARNING("Can't create grey copy");
        return;
    }

    cv::Mat binaryCopy;
    cv::threshold(greyCopy, binaryCopy, 128, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat croppedCopy = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(255, 255, 255));
    cv::drawContours(croppedCopy, contours, -1, cv::Scalar(0, 0, 0), 1);
    return croppedCopy;
}

double Analyse::ImageObject::templateCompare(cv::Mat& img, cv::Mat& templateImg, bool withOutput)
{
    if (img.empty())
    {
        LOG_OPRES_ERROR("Can't read image");
        return double();
    }

    cv::Mat result;
    double minVal {}, maxVal {};
    cv::Point minLoc, maxLoc;

    // Compare
//    cv::matchTemplate(img, templateImg, result, cv::TM_CCOEFF_NORMED);
    cv::matchTemplate(img, templateImg, result, cv::TM_CCORR_NORMED);
//    cv::matchTemplate(img, templateImg, result, cv::TM_SQDIFF_NORMED);
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // Show what fe cound
    if (withOutput)
    {
        cv::Mat croppedCopy = img;
        cv::rectangle(croppedCopy, maxLoc, cv::Point(maxLoc.x + templateImages[0].cols, maxLoc.y + templateImages[0].rows), cv::Scalar(0, 0, 255));
        cv::imwrite(tempDirPath + "/found_[" + this->name + "] with percent [ " + std::to_string(maxVal) + " ].svg", croppedCopy);
        return maxVal;
    }

    // Check compare result
    return maxVal;
}

cv::Mat Analyse::ImageObject::setupTemplate(const std::string &templateFilePath)
{
    // Read image
    cv::Mat result = cv::imread(filePath);
    if (inputImg.empty())
        return {};

    cv::Mat greyResult;
    cv::cvtColor(result, greyResult, cv::COLOR_BGR2GRAY);
    return greyResult;
}

bool Analyse::ImageObject::operator !=(const Analyse::ImageObject &ot)
{
    return (ot.name == name);
}

float Analyse::ImageObject::templateMatch(const std::string &imageFilePath, const std::string &templateFilePath)
{
    std::vector<cv::Mat> templateImages {setupTemplate(templateFilePath)};
    if (templateImages[0].empty())
        return 0;

    // Setup image
    cv::Mat coloredCompareImage = cv::imread(filePath);
    cv::Mat greyCompareImage;
    cv::cvtColor(coloredCompareImage, greyCompareImage, cv::COLOR_BGR2GRAY);
    double maxMatch {0};

    // Compare using image copyies
    for (size_t i = 0 ; i < templateImages.size(); i++)
    {
        double compRes = compare(greyCompareImage, templateImages[i]);
        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}
