#include "common.hpp"

#include "logging.hpp"

namespace Common
{

// Describes how small object can be to process it
constexpr size_t MINIMAL_OBJECT_SIZE = 30 * 30;

cv::Mat loadImage(const std::string &filepath)
{
    // Read image
    cv::Mat imageRead = cv::imread(filepath, cv::IMREAD_GRAYSCALE);
    if (imageRead.empty())
    {
        LOG_OPRES_ERROR("Can't read template by path: %s", filepath.c_str());
        return {};
    }
//    cv::imwrite("image.png", imageRead);

    // Able morpho filter
    cv::Mat resultImage;
//    cv::adaptiveThreshold(imageRead, resultImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);
//    cv::imwrite("result_aThreshold.png", resultImage);

//    cv::Mat tempImage;
//    cv::morphologyEx(imageRead, resultImage, cv::MORPH_GRADIENT, tempImage);
//    cv::imwrite("result_morpho.png", resultImage);

//    cv::Laplacian(imageRead, resultImage, 5);
//    cv::imwrite("result_lapla.png", resultImage);

//    cv::Canny(imageRead, resultImage, 200, 100, 3);
//    cv::imwrite("result_canny.png", resultImage);

//    cv::Mat markersImage = cv::imread("shapedM.png");
//    cv::watershed(imageRead, markersImage);
//    cv::imwrite("result_markers.png", imageRead);
    return imageRead;
}

void addContours(cv::Mat &img, ContoursType &imageContours)
{
    // Search into temporary array
    ContoursType tempRes;
    cv::findContours(img, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // If nothing found, return
    if (!tempRes.size())
        return;

    // Search for largest contour
    std::vector<cv::Point>* largestContour;
    double currentArea {0};

    // If nothing found or just one countour in array, will be set to first
    largestContour = &tempRes[0];
    for (auto& contour : tempRes)
    {
        // Count area of contour
        double tempArea = cv::contourArea(contour);
        if (currentArea < tempArea)
        {
            currentArea = tempArea;
            largestContour = &contour;
        }
    }

    // Add only largest contour into array
    imageContours.push_back(*largestContour);
}

void drawFound(cv::Mat &img, ContoursType &foundObjectContours, const std::string &baseName)
{
    // Save image objects
    int no = 1;
    for (auto& contour : foundObjectContours)
    {
        cv::Rect boundingRect = cv::boundingRect(contour);
        cv::Mat contouredObject = img(boundingRect);
        cv::imwrite(baseName + "_Object-" + std::to_string(no++) + ".png", contouredObject);
    }
}

std::vector<cv::Mat> getObjects(const std::string &imageFullPath)
{
    std::vector<cv::Mat> result;
    try
    {
        // Get image
        cv::Mat mainImage = Common::loadImage(imageFullPath);
        if (mainImage.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }

        // Image binarization
//        cv::threshold(mainImage, mainImage, 155, 255, cv::THRESH_BINARY);
//        cv::adaptiveThreshold(mainImage, mainImage, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 3, 0);
        cv::adaptiveThreshold(mainImage, mainImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);

        // Search for objects
        Common::ContoursType contours;
        Common::addContours(mainImage, contours);

        result.resize(contours.size());

        // Get objects array from contours
        size_t currentIndex = 0;
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);
            if (boundingRect.area() < MINIMAL_OBJECT_SIZE) continue;
            result[currentIndex] = mainImage(boundingRect);
        }

        // Remove unused positions
        auto resultEndR = std::find_if(result.rbegin(), result.rend(), [](auto& img){ return !img.empty(); });
        auto resultEnd = resultEndR.base();
        if (resultEnd != result.end()) result.erase(resultEnd + 1, result.end()); // +1 because iterator points to non-zero object

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
}

}
