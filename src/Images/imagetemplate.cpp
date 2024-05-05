#include "imagetemplate.hpp"

#include "logging.hpp"

#include "common.hpp"

namespace Analyse
{


ImageComparator::ImageComparator()
{

}


ImageComparator::ImageComparator(const ImageComparator &imgTemplate)
{
    m_typeName = imgTemplate.m_typeName;
    m_templateFilePath = imgTemplate.m_templateFilePath;
    m_templateRotations = imgTemplate.m_templateRotations;
    m_loadedTemplateImage = imgTemplate.m_loadedTemplateImage;
    m_contours = imgTemplate.m_contours;
}


ImageComparator::~ImageComparator()
{

}


ImageComparator &ImageComparator::operator =(const ImageComparator &imgTemplate)
{
    m_typeName = imgTemplate.m_typeName;
    m_templateFilePath = imgTemplate.m_templateFilePath;
    m_templateRotations = imgTemplate.m_templateRotations;
    m_loadedTemplateImage = imgTemplate.m_loadedTemplateImage;
    return *this;
}


void ImageComparator::setTemplate(const std::string &filepath)
{
    m_templateFilePath = filepath;
    m_loadedTemplateImage = Common::loadImage(filepath);
    setupRotations();
}


std::string ImageComparator::getTemplate() const
{
    return m_templateFilePath;
}


void ImageComparator::setName(const std::string &name)
{
    m_typeName = name;
}


std::string ImageComparator::getName() const
{
    return m_typeName;
}


double ImageComparator::bestMatch(cv::Mat &img)
{
    if ((img.cols > m_loadedTemplateImage.cols) || (img.rows > m_loadedTemplateImage.rows))
    {
        return matchHist(img);
    }

    return matchTemplate(img);
}


double ImageComparator::matchTemplate(cv::Mat &img)
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

double ImageComparator::matchHist(cv::Mat &img)
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

double ImageComparator::matchContour(cv::Mat &img)
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

    try {
        double resultMatch {1}; // To not use infinity value
        for (auto& contour : m_contours)
        {
            for (auto& imageContour : imageContours)
            {
                double tempMatchRes = cv::matchShapes(contour, imageContour, cv::CONTOURS_MATCH_I2, 0);
                tempMatchRes = 1.0 - tempMatchRes;
//                LOG_DEBUG("[%s] Compare result: %f", m_templateName.c_str(), tempMatchRes);
                if (tempMatchRes > resultMatch)
                {
                    resultMatch = tempMatchRes;
                }
            }
        }
        return resultMatch;
    } catch (cv::Exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return 0;
}


void ImageComparator::setupRotations()
{
    m_templateRotations.push_back(m_loadedTemplateImage);
    Common::addContours(m_loadedTemplateImage, m_contours);

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
    m_contours.erase(std::unique(m_contours.begin(), m_contours.end(), [](auto& cont1, auto& cont2){ return (cv::matchShapes(cont1, cont2, cv::CONTOURS_MATCH_I2, 0) < 1.0); }), m_contours.end());

//    std::string basepath = "templ_rots/";
//    basepath = basepath + m_templateName + "-";

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


double ImageComparator::match(cv::Mat &img, cv::Mat &templateImage)
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


void ImageComparator::createRotations(size_t &currentIndex)
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
