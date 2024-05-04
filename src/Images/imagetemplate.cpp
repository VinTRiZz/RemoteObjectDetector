#include "imagetemplate.hpp"

#include "logging.hpp"

#include "common.hpp"

namespace Analyse
{


ImageTemplate::ImageTemplate()
{

}


ImageTemplate::ImageTemplate(const ImageTemplate &imgTemplate)
{
    m_templateName = imgTemplate.m_templateName;
    m_templateFilePath = imgTemplate.m_templateFilePath;
    m_templateRotations = imgTemplate.m_templateRotations;
    m_loadedTemplateImage = imgTemplate.m_loadedTemplateImage;
}


ImageTemplate::~ImageTemplate()
{

}


ImageTemplate &ImageTemplate::operator =(const ImageTemplate &imgTemplate)
{
    m_templateName = imgTemplate.m_templateName;
    m_templateFilePath = imgTemplate.m_templateFilePath;
    m_templateRotations = imgTemplate.m_templateRotations;
    m_loadedTemplateImage = imgTemplate.m_loadedTemplateImage;
    return *this;
}


void ImageTemplate::setTemplate(const std::string &filepath)
{
    m_templateFilePath = filepath;
    m_loadedTemplateImage = Common::loadImage(filepath);
}


std::string ImageTemplate::getTemplate() const
{
    return m_templateFilePath;
}


void ImageTemplate::setName(const std::string &name)
{
    m_templateName = name;
}


std::string ImageTemplate::getName() const
{
    return m_templateName;
}


double ImageTemplate::match(const std::string &filepath)
{
    if (m_loadedTemplateImage.empty())
    {
        LOG_ERROR("Template load error. Path: %s", m_templateFilePath.c_str());
        return 0;
    }

    if (!m_templateRotations.size())
        setupRotations();

    // Setup image
    cv::Mat compareImage = Common::loadImage(filepath);
    if (compareImage.empty())
    {
        LOG_OPRES_ERROR("Image load error. Path: %s", filepath.c_str());
        return 0;
    }

    double maxMatch {0};

    // Compare using image copyies
    for (auto& templateRotation : m_templateRotations)
    {
        double compRes = match(compareImage, templateRotation);
        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}


double ImageTemplate::matchLoaded(cv::Mat &img)
{
    if (m_loadedTemplateImage.empty())
    {
        LOG_ERROR("Template load error. Path: %s", m_templateFilePath.c_str());
        return 0;
    }

    if (!m_templateRotations.size())
        setupRotations();

    // Setup image
    if (img.empty())
    {
        LOG_OPRES_ERROR("Null image inserted to match searcher");
        return 0;
    }

    double maxMatch {0};

    // Compare using image copyies
    for (auto& templateRotation : m_templateRotations)
    {
        double compRes = match(img, templateRotation);
        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}


void ImageTemplate::setupRotations()
{
    m_templateRotations.push_back(m_loadedTemplateImage);

    size_t currentIndex = 0;

    // Normal rotations
    createRotations(currentIndex);

    // Mirror vertically
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 0);

    // Mirrored vertical
    createRotations(currentIndex);

    // Mirror horizontally
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 1);

    // Mirrored horizontal
    createRotations(currentIndex);

    std::remove_if(m_templateRotations.begin(), m_templateRotations.end(), [](auto& matr){ return matr.empty(); });
}


double ImageTemplate::match(cv::Mat &img, cv::Mat &templateImage)
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


void ImageTemplate::createRotations(size_t &currentIndex)
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

}
