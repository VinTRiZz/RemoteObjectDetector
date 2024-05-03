#include "imagetemplate.hpp"

#include <opencv2/opencv.hpp>

#include "logging.hpp"

namespace Analyse
{

struct ImageTemplate::ImageTemplatePrivate
{
    std::string m_templateFilePath; // Path to localfile
    cv::Mat m_loadedTemplateImage; // Main image
    std::vector<cv::Mat> m_templateRotations; // Optimisation for next time compare
    std::string m_templateName {"Unknown"}; // Name for template, registered in system

    cv::Mat loadImage(const std::string& filepath)
    {
        // Read image
        cv::Mat result = cv::imread(filepath);
        if (result.empty())
        {
            LOG_OPRES_ERROR("Can't read template by path: %s", filepath.c_str());
            return {};
        }

        cv::Mat greyResult;
        cv::cvtColor(result, greyResult, cv::COLOR_BGR2GRAY);
        return greyResult;
//        return result;
    }

    double match(cv::Mat& img, cv::Mat& templateImage)
    {
        if (img.empty() || templateImage.empty())
        {
            LOG_OPRES_ERROR("Null image put in match function");
            return 0;
        }

        double minVal {}, maxVal {};

        cv::Mat result;
        cv::Point minLoc, maxLoc;

        // Compare
        try {
//            cv::matchTemplate(img, templateImage, result, cv::TM_CCOEFF_NORMED);
            cv::matchTemplate(img, templateImage, result, cv::TM_CCORR_NORMED);
//            cv::matchTemplate(img, templateImage, result, cv::TM_SQDIFF_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            // Show if anything found
//            cv::Mat croppedCopy = img;
//            cv::rectangle(croppedCopy, maxLoc, cv::Point(maxLoc.x + templateImage.cols, maxLoc.y + templateImage.rows), cv::Scalar(0, 0, 255));
//            cv::imwrite(filepath + " : " + m_templateName + " [ " + std::to_string(maxVal) + " ].png", croppedCopy);

        } catch (cv::Exception& ex) {
            LOG_OPRES_ERROR("Got OpenCV exception: %s", ex.what());
            return 0;
        }

        // Check compare result
        return maxVal;
    }

    void createRotations(size_t& currentIndex)
    {
        // Rotate 90
        m_templateRotations.push_back({});
        cv::transpose(m_templateRotations[currentIndex], m_templateRotations[currentIndex + 1]);
        cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 1);

        // Rotate -90
        m_templateRotations.push_back({});
        cv::transpose(m_templateRotations[currentIndex], m_templateRotations[currentIndex + 2]);
        cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 0);

        // Rotate 180
        m_templateRotations.push_back({});
        cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex + 3], -1);

        currentIndex += 4;
    }
};

ImageTemplate::ImageTemplate() :
    d {new ImageTemplatePrivate}
{

}

ImageTemplate::ImageTemplate(const ImageTemplate &imgTemplate) :
    d {new ImageTemplatePrivate}
{
    d->m_templateName = imgTemplate.d->m_templateName;
    d->m_templateFilePath = imgTemplate.d->m_templateFilePath;
    d->m_templateRotations = imgTemplate.d->m_templateRotations;
    d->m_loadedTemplateImage = imgTemplate.d->m_loadedTemplateImage;
}

ImageTemplate::~ImageTemplate()
{

}

ImageTemplate &ImageTemplate::operator =(const ImageTemplate &imgTemplate)
{
    d->m_templateName = imgTemplate.d->m_templateName;
    d->m_templateFilePath = imgTemplate.d->m_templateFilePath;
    d->m_templateRotations = imgTemplate.d->m_templateRotations;
    d->m_loadedTemplateImage = imgTemplate.d->m_loadedTemplateImage;
    return *this;
}

void ImageTemplate::setTemplate(const std::string &filepath)
{
    d->m_templateFilePath = filepath;
    d->m_loadedTemplateImage = d->loadImage(filepath);
}

std::string ImageTemplate::getTemplate() const
{
    return d->m_templateFilePath;
}

void ImageTemplate::setName(const std::string &name)
{
    d->m_templateName = name;
}

std::string ImageTemplate::getName() const
{
    return d->m_templateName;
}

double ImageTemplate::match(const std::string &filepath)
{
    if (d->m_loadedTemplateImage.empty())
    {
        LOG_ERROR("Template load error. Path: %s", d->m_templateFilePath.c_str());
        return 0;
    }

    if (!d->m_templateRotations.size())
        setupRotations();

    // Setup image
    cv::Mat compareImage = d->loadImage(filepath);
    if (compareImage.empty())
    {
        LOG_OPRES_ERROR("Image load error. Path: %s", filepath.c_str());
        return 0;
    }

    double maxMatch {0};

    // Compare using image copyies
    for (auto& templateRotation : d->m_templateRotations)
    {
        double compRes = d->match(compareImage, templateRotation);
        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}

void ImageTemplate::setupRotations()
{
    d->m_templateRotations.push_back(d->m_loadedTemplateImage);

    size_t currentIndex = 0;

    // Normal rotations
    d->createRotations(currentIndex);

    // Mirror vertically
    d->m_templateRotations.push_back({});
    cv::flip(d->m_templateRotations[0], d->m_templateRotations[currentIndex], 0);

    // Mirrored vertical
    d->createRotations(currentIndex);

    // Mirror horizontally
    d->m_templateRotations.push_back({});
    cv::flip(d->m_templateRotations[0], d->m_templateRotations[currentIndex], 1);

    // Mirrored horizontal
    d->createRotations(currentIndex);

    std::remove_if(d->m_templateRotations.begin(), d->m_templateRotations.end(), [](auto& matr){ return matr.empty(); });
}

}
