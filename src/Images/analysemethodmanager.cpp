#include "analysemethodmanager.hpp"

namespace Analyse
{

AnalyseMethodManager::AnalyseMethodManager()
{

}

double AnalyseMethodManager::compareMoments(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    // Get Hu moments of contours
    std::vector<double> imageMomentsVect = Common::createMoments(image);

    // Compare moments of image with type moments
    std::vector<double> compareResults(typeIHolder.moments.size());

    std::transform(typeIHolder.moments.begin(), typeIHolder.moments.end(), compareResults.begin(), [&](auto& momentsVect){ return cv::matchShapes(imageMomentsVect, momentsVect, 1, 0); });
    std::sort(compareResults.begin(), compareResults.end());
    const size_t countOfItems = compareResults.size();
    const double maxVal = *compareResults.begin();
    const double minVal = *(compareResults.end() - 1);
    const double diff = maxVal - minVal;
    std::transform(compareResults.begin(), compareResults.end(), compareResults.begin(), [&](auto val){ return (val - minVal) / diff; });

    if (!compareResults.size()) return 0;

    LOG_DEBUG("Comp res: %f", *(compareResults.end() - 1));

    return *(compareResults.end() - 1);
}

double AnalyseMethodManager::compareHistogram(const Common::TypeInfoHolder &typeIHolder, const cv::Mat &image)
{
    // Setup histogram configuration
    cv::Mat imgHist;                     // Histograms
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    if (image.channels() > 0)
        return 0;

    // Calculate histogram for input image
    cv::calcHist(&image, 1, 0, cv::Mat(), imgHist, image.dims, &histSize, &histRange);

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
