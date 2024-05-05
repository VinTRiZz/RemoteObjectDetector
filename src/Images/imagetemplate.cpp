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

double ImageTemplate::matchHist(cv::Mat &img)
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

    cv::Mat templateHist, imgHist;
    int histSize = 256; // Number of bins
    float range[] = {0, 255}; // Range of pixel values
    const float* histRange = {range};

    cv::calcHist(&img, 1, 0, cv::Mat(), imgHist, 1, &histSize, &histRange);

    for (auto& templateRotation : m_templateRotations)
    {
        cv::calcHist(&templateRotation, 1, 0, cv::Mat(), templateHist, 1, &histSize, &histRange);
        double compRes = cv::compareHist(templateHist, imgHist, cv::HISTCMP_CORREL);

        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}

double ImageTemplate::matchContour(cv::Mat &img)
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

    // Get contour of an object
    ContoursType imageContours;
    Common::addContours(img, imageContours);

    double imageContourArea = cv::contourArea(imageContours[0]);

    try {
        double resultMatch {0};
        for (auto& contour : m_contours)
        {
            double templateContourArea = cv::contourArea(contour);
            double diff = std::abs(imageContourArea - templateContourArea);
            if (diff <= 100)
            {
                double tempMatchRes = cv::matchShapes(m_contours, imageContours, cv::CONTOURS_MATCH_I2, 0);
                if (tempMatchRes > resultMatch)
                {
                    resultMatch = tempMatchRes;
                    LOG_OPRES_SUCCESS("Compared 2 vectors!");
                    continue;
                }
                LOG_OPRES_ERROR("Not compared");
                continue;
            }
            LOG_OPRES_ERROR("Diff more than 100: %f", diff);
        }
    } catch (cv::Exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return 0;
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
    Common::addContours(m_templateRotations[currentIndex], m_contours);

    // Mirrored vertical
    createRotations(currentIndex);

    // Mirror horizontally
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 1);
    Common::addContours(m_templateRotations[currentIndex], m_contours);

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
        double resultVal {0};

        cv::matchTemplate(img, templateImage, result, cv::TM_CCOEFF_NORMED);
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
        resultVal += maxVal / 3.0f;

        cv::matchTemplate(img, templateImage, result, cv::TM_CCORR_NORMED);
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
        resultVal += maxVal / 3.0f;

        cv::matchTemplate(img, templateImage, result, cv::TM_SQDIFF_NORMED);
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
        resultVal += (1.0 - maxVal) / 3.0f;

        maxVal = resultVal;

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
    Common::addContours(m_templateRotations[currentIndex], m_contours);

    // Rotate -90
    currentIndex++;
    m_templateRotations.push_back({});
    cv::transpose(m_templateRotations[currentIndex - 2], m_templateRotations[currentIndex]);
    cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 0);
    Common::addContours(m_templateRotations[currentIndex], m_contours);

    // Rotate 180
    currentIndex++;
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[currentIndex - 3], m_templateRotations[currentIndex], -1);
    Common::addContours(m_templateRotations[currentIndex], m_contours);

    currentIndex++;
}

}
