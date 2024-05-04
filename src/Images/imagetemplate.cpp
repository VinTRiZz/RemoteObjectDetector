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
    setupRotations();
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

    // Setup image
    if (img.empty())
    {
        LOG_OPRES_ERROR("Null image inserted to match searcher");
        return 0;
    }

    double maxMatch {0};

    // Compare using image copyies
    int no = 0;
    for (auto& templateRotation : m_templateRotations)
    {
        double compRes = match(img, templateRotation);
        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}

double ImageTemplate::matchContours(cv::Mat &img)
{
    if (m_loadedTemplateImage.empty())
    {
        LOG_ERROR("Template load error. Path: %s", m_templateFilePath.c_str());
        return 0;
    }

    // Setup image
    if (img.empty())
    {
        LOG_OPRES_ERROR("Null image inserted to match searcher");
        return 0;
    }

    double maxMatch {0};

    // Get contour of an object
    ContoursType imageContours;
    cv::findContours(img, imageContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Compare using image copyies contours
    int no = 0;
    for (auto& templateContour : m_contours)
    {
        for (auto& imageContour : imageContours)
        {
            try {
                double compRes = cv::matchShapes(templateContour, imageContour, cv::CONTOURS_MATCH_I1, 0);
                if (compRes > maxMatch)
                    maxMatch = compRes;
            } catch (cv::Exception& ex) {
                LOG_ERROR("Got OpenCV exception: %s", ex.what());
            }
        }
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
    addContours(m_templateRotations[currentIndex]);

    // Mirrored vertical
    createRotations(currentIndex);

    // Mirror horizontally
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 1);
    addContours(m_templateRotations[currentIndex]);

    // Mirrored horizontal
    createRotations(currentIndex);

    // Remove bad
    std::remove_if(m_templateRotations.begin(), m_templateRotations.end(), [](auto& matr){ return matr.empty(); });

    // Remove dublicates


    const std::string basepath = "templ_rots/";

//    cv::imwrite(basepath + "Temp_normal.png", m_templateRotations[0]);
//    cv::imwrite(basepath + "Temp_mirrV.png", m_templateRotations[4]);
//    cv::imwrite(basepath + "Temp_mirrH.png", m_templateRotations[8]);

//    cv::imwrite(basepath + std::to_string(1) + "_N_Temp_90.png", m_templateRotations[1]);
//    cv::imwrite(basepath + std::to_string(2) + "_N_Temp_-90.png", m_templateRotations[2]);
//    cv::imwrite(basepath + std::to_string(3) + "_N_Temp_180.png", m_templateRotations[3]);

//    cv::imwrite(basepath + std::to_string(5) + "_MV_Temp_90.png", m_templateRotations[5]);
//    cv::imwrite(basepath + std::to_string(6) + "_MV_Temp_-90.png", m_templateRotations[6]);
//    cv::imwrite(basepath + std::to_string(7) + "_MV_Temp_180.png", m_templateRotations[7]);

//    cv::imwrite(basepath + std::to_string(9) + "_MH_Temp_90.png", m_templateRotations[9]);
//    cv::imwrite(basepath + std::to_string(10) + "_MH_Temp_-90.png", m_templateRotations[10]);
//    cv::imwrite(basepath + std::to_string(11) + "_MH_Temp_180.png", m_templateRotations[11]);
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

    } catch (cv::Exception& ex) {
//        LOG_OPRES_ERROR("Got OpenCV exception: %s", ex.what());
        return 0;
    }

    // Check compare result
    return maxVal;
}


void ImageTemplate::createRotations(size_t &currentIndex)
{
    // Rotate 90
    currentIndex++;
    m_templateRotations.push_back({});
    cv::transpose(m_templateRotations[currentIndex - 1], m_templateRotations[currentIndex]);
    cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 1);
    addContours(m_templateRotations[currentIndex]);

    // Rotate -90
    currentIndex++;
    m_templateRotations.push_back({});
    cv::transpose(m_templateRotations[currentIndex - 2], m_templateRotations[currentIndex]);
    cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 0);
    addContours(m_templateRotations[currentIndex]);

    // Rotate 180
    currentIndex++;
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[currentIndex - 3], m_templateRotations[currentIndex], -1);
    addContours(m_templateRotations[currentIndex]);

    currentIndex++;
}

void ImageTemplate::addContours(cv::Mat &img)
{
    ContoursType imageContours;
    cv::findContours(img, imageContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    m_contours.push_back(imageContours);
}

}
