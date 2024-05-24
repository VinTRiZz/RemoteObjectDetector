#include "analysemethodmanager.hpp"

namespace Analyse
{

AnalyseMethodManager::AnalyseMethodManager()
{

}

double AnalyseMethodManager::compareContour(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    return 0; // TODO: Write-up
}

double AnalyseMethodManager::compareMoments(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    // Get Hu moments of contours
    cv::Moments imageMoments = cv::moments(image);
    std::vector<double> imageMomentsVect;
    cv::HuMoments(imageMoments, imageMomentsVect);

    // Compare moments of image with type moments
    double resultMatch = 0;
    for (auto& momentsVect : typeIHolder.moments)
    {
        double tempMatch = cv::matchShapes(imageMomentsVect, momentsVect, 1, 0);
        if (tempMatch > resultMatch) resultMatch = tempMatch;
    }

    return resultMatch;
}

double AnalyseMethodManager::compareHistogram(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    // Setup histogram configuration
    cv::Mat imgHist;                     // Histograms
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    // Calculate histogram for input image
    cv::calcHist(&image, 1, 0, cv::Mat(), imgHist, 1, &histSize, &histRange);

    // Compare histograms
    double maxMatch {0};
    for (auto& templateHist : typeIHolder.histograms)
    {
        double compRes = cv::compareHist(templateHist, imgHist, cv::HISTCMP_CORREL);

        if (compRes > maxMatch)
            maxMatch = compRes;
    }

    return maxMatch;
}

double AnalyseMethodManager::compareTemplate(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    double maxMatch {0};
    double minVal {}, maxVal {};

    cv::Mat result;
    cv::Point minLoc, maxLoc;

    // Compare using image copyies
    for (auto& templateRotation : typeIHolder.imageRotations)
    {
        // Compare
        try {
            double resultVal {0};

            cv::matchTemplate(image, templateRotation, result, cv::TM_CCOEFF_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += maxVal / 3.0f;

            cv::matchTemplate(image, templateRotation, result, cv::TM_CCORR_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += maxVal / 3.0f;

            cv::matchTemplate(image, templateRotation, result, cv::TM_SQDIFF_NORMED);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            resultVal += (1.0 - maxVal) / 3.0f;

            if (resultVal > maxMatch)
                maxMatch = resultVal;

        } catch (cv::Exception& ex) {
            continue; // Skip error image
        }
    }

    return maxMatch;
}

}
