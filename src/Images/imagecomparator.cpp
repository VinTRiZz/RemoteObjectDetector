#include "imagecomparator.hpp"

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


void ImageComparator::setImage(const std::string &filepath)
{
    m_templateFilePath = filepath;
    m_loadedTemplateImage = Common::loadImage(filepath);
    m_contours.clear();
    m_templateRotations.clear();
    m_histograms.clear();
    setupRotations();
}


std::string ImageComparator::getImagePath() const
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


double ImageComparator::bestMatch(cv::Mat &img, ImageCompareMethod compMethod)
{
    switch (compMethod)
    {
    case ImageCompareMethod::IMAGE_COMPARE_METHOD_TEMPLATE:
        return matchTemplate(img);

    case ImageCompareMethod::IMAGE_COMPARE_METHOD_HIST:
        return matchHist(img);

    case ImageCompareMethod::IMAGE_COMPARE_METHOD_CONTOUR:
        return matchContour(img);
    }

    return 0;
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

    double minVal {}, maxVal {};

    cv::Mat result;
    cv::Point minLoc, maxLoc;

    // Compare using image copyies
    int no = 0;
    for (auto& templateRotation : m_templateRotations)
    {
        // Compare
        try {
            double resultVal {0};

            cv::matchTemplate(img, templateRotation, result, cv::TM_CCOEFF_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += maxVal / 3.0f;

            cv::matchTemplate(img, templateRotation, result, cv::TM_CCORR_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += maxVal / 3.0f;

            cv::matchTemplate(img, templateRotation, result, cv::TM_SQDIFF_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += (1.0 - maxVal) / 3.0f;

            if (resultVal > maxMatch)
                maxMatch = resultVal;

        } catch (cv::Exception& ex) {
            continue;
        }
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

    // Setup histogram configuration
    cv::Mat imgHist;                     // Histograms
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    // Calculate histogram for input image
    cv::calcHist(&img, 1, 0, cv::Mat(), imgHist, 1, &histSize, &histRange);

    // If not calculated yet, calculate template histograms
    if (!m_histograms.size())
        setupHistograms();

    // Compare histograms
    for (auto& templateHist : m_histograms)
    {
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

    // Setup contours if not yet
    if (!m_contours.size())
    {
        // Create contours
        for (auto& templateImage : m_templateRotations)
            Common::addContours(templateImage, m_contours);

        // Remove dublicates
        m_contours.erase(std::unique(m_contours.begin(), m_contours.end(), [](auto& cont1, auto& cont2){ return (cv::matchShapes(cont1, cont2, cv::CONTOURS_MATCH_I2, 0) < 1.0); }), m_contours.end());
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

    // Compare contours using three methods
    double resultMatch {1}; // To not use infinity value
    for (auto& contour : m_contours)
    {
        for (auto& imageContour : imageContours)
        {
            // Integral invariant count for contours
            double tempMatchRes1 = cv::matchShapes(contour, imageContour, cv::CONTOURS_MATCH_I1, 0);
            tempMatchRes1 = 1.0 - tempMatchRes1;
            if (tempMatchRes1 > resultMatch)
                resultMatch = tempMatchRes1;

            // Count equal metrics for contours
            double tempMatchRes2 = cv::matchShapes(contour, imageContour, cv::CONTOURS_MATCH_I2, 0);
            tempMatchRes2 = 1.0 - tempMatchRes2;
            if (tempMatchRes2 > resultMatch)
                resultMatch = tempMatchRes2;

            // Shape Context Matching algorithm (using histograms of contours)
            double tempMatchRes3 = cv::matchShapes(contour, imageContour, cv::CONTOURS_MATCH_I3, 0);
            tempMatchRes3 = 1.0 - tempMatchRes3;
            if (tempMatchRes3 > resultMatch)
                resultMatch = tempMatchRes3;
        }
    }

    return resultMatch;
}


void ImageComparator::setupRotations()
{
    // Setup first index (image by itself)
    m_templateRotations.push_back(m_loadedTemplateImage);

    // Index to setup other images
    size_t currentIndex = 0;

    // Rotations for normal image
    createRotations(currentIndex);

    // Mirror image vertical
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 0);

    // Rotations for mirrored vertical
    createRotations(currentIndex);

    // Mirror image horizontal
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[0], m_templateRotations[currentIndex], 1);

    // Rotations for mirrored horizontal
    createRotations(currentIndex);

    // Remove empty rotation results if exist
    std::remove_if(m_templateRotations.begin(), m_templateRotations.end(), [](auto& matr){ return matr.empty(); });
}



void ImageComparator::createRotations(size_t &currentIndex)
{
    // Rotate 90
    currentIndex++;
    m_templateRotations.push_back({});
    cv::transpose(m_templateRotations[currentIndex - 1], m_templateRotations[currentIndex]);
    cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 1);

    // Rotate -90
    currentIndex++;
    m_templateRotations.push_back({});
    cv::transpose(m_templateRotations[currentIndex - 2], m_templateRotations[currentIndex]);
    cv::flip(m_templateRotations[currentIndex], m_templateRotations[currentIndex], 0);

    // Rotate 180
    currentIndex++;
    m_templateRotations.push_back({});
    cv::flip(m_templateRotations[currentIndex - 3], m_templateRotations[currentIndex], -1);

    currentIndex++;
}

void ImageComparator::setupHistograms()
{
    // Setup histogram configuration
    cv::Mat templateHist;               // Histogram
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    // Calculate histograms
    for (auto& templateRotation : m_templateRotations)
    {
        cv::calcHist(&templateRotation, 1, 0, cv::Mat(), templateHist, 1, &histSize, &histRange);
        m_histograms.push_back(templateHist);
    }
}

}
