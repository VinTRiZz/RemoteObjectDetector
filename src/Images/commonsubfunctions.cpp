#include "common.hpp"

namespace Common
{

inline void addContours(const cv::Mat &img, std::vector<ContourType> &imageIHolder)
{
    // Search into temporary array
    std::vector<ContourType> tempRes;
    cv::findContours(img, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // If nothing found, return
    if (!tempRes.size()) return;

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
    imageIHolder.push_back(*largestContour);
}

inline std::vector<double> createMoments(const cv::Mat& image)
{
    std::vector<double> momentsVect;
    cv::HuMoments(cv::moments(image), momentsVect);
    return momentsVect;
}

inline void createRotations(std::vector<cv::Mat>& result, size_t &currentIndex)
{
    auto& baseImage = result[currentIndex];
    currentIndex++;
    auto* pImage = &result[currentIndex];

    // Rotate 90
    cv::transpose(baseImage, *pImage);
    cv::flip(*pImage, *pImage, 1);

    // Rotate -90
    currentIndex++;
    pImage = &result[currentIndex];
    cv::transpose(baseImage, *pImage);
    cv::flip(*pImage, *pImage, 0);

    // Rotate 180
    currentIndex++;
    cv::flip(baseImage, result[currentIndex], -1);

    currentIndex++;
}


inline std::vector<cv::Mat> getRotations(const cv::Mat& image)
{
    std::vector<cv::Mat> result;

    // Setup first index and reserve memory for other
    result.push_back(image);
    result.resize(3 * 4);

    // Temp values
    size_t currentIndex = 0;

    // Rotations for normal image
    createRotations(result, currentIndex);

    // Mirror image vertical
    cv::flip(result[0], result[currentIndex], 0);

    // Rotations for mirrored vertical
    createRotations(result, currentIndex);

    // Mirror image horizontal
    cv::flip(result[0], result[currentIndex], 1);

    // Rotations for mirrored horizontal
    createRotations(result, currentIndex);

    // Remove empty rotation results if exist
    result.erase(std::remove_if(result.begin(), result.end(), [](auto& matr){ return matr.empty(); }), result.end());

    return result;
}


inline std::vector<std::vector<double> > getMoments(const cv::Mat &image)
{
    // Setup first index and reserve memory for other
    auto rotations = getRotations(image);
    std::vector<std::vector<double> > result(rotations.size());
    std::transform(rotations.begin(), rotations.end(), result.begin(), [](auto& img){ return createMoments(img); });

    // Remove empty rotation results if exist
    std::remove_if(result.begin(), result.end(), [](auto& mom){ return mom.empty(); });

    return result;
}

inline std::list<cv::Mat> getHistograms(const std::vector<cv::Mat> &imageRotations)
{
    std::list<cv::Mat> histograms;

    // Setup histogram configuration
    cv::Mat templateHist;               // Histogram
    int histSize = 256;                 // Number of bins
    float range[] = {0, 255};           // Range of pixel values
    const float* histRange = {range};   // Range of histogram

    // TODO: Calc PGH instead of usual histograms

    // Calculate histograms
    size_t currentIndex = 0;
    for (auto& templateRotation : imageRotations)
    {
        cv::calcHist(&templateRotation, 1, 0, cv::Mat(), templateHist, 1, &histSize, &histRange);
        histograms.push_back(templateHist);
    }
    return histograms;
}

inline void setupInfoHolder(TypeInfoHolder &imageIHolder)
{
    imageIHolder.image      = loadImage(imageIHolder.imagePath);
    imageIHolder.imageRotations = getRotations(imageIHolder.image);

    addContours(imageIHolder.image, imageIHolder.contours);
    imageIHolder.moments    = getMoments(imageIHolder.image);
}

inline void addType(std::list<TypeInfoHolder> &typeList, const std::string& typeName)
{
    TypeInfoHolder typeIHolder;
    typeIHolder.typeName = typeName;

    // Check if type already exist
    auto exist = std::find(typeList.begin(), typeList.end(), typeIHolder);
    if (exist != typeList.end()) return;

    // Add if not exist
    typeList.push_front(typeIHolder);
}

}
