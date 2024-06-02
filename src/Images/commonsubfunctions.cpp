#include "common.hpp"

namespace Common
{

inline void addContours(const cv::Mat &img, std::vector<ContourType> &imageContours)
{
    cv::Mat grayImage;
    if ((img.channels() > 2) && (img.type() != CV_8UC1))
        cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY);
    else
        grayImage = img;

    // Search into temporary array
    std::vector<ContourType> tempRes;
    cv::findContours(grayImage, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // If nothing found, return
    if (!tempRes.size()) return;

    imageContours.resize(imageContours.size() + tempRes.size());
    std::copy(tempRes.begin(), tempRes.end(), imageContours.begin());
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


inline void setupInfoHolder(TypeInfoHolder &imageIHolder, cv::Ptr<cv::BackgroundSubtractor>& pBackgroundSub)
{
    // Setup object things
    imageIHolder.image          = loadImage(imageIHolder.imagePath);

    auto objects                = getObjects(imageIHolder.image, pBackgroundSub);
    if (objects.size())
    {
        imageIHolder.image = objects[0];
        LOG_DEBUG("%s Inserted object found", imageIHolder.typeName.c_str());

        int no = 1;
        for (auto& img : objects)
        {
            cv::imwrite(std::string("F_") + imageIHolder.typeName + "_" + std::to_string(no++) + ".png", img);
        }
        cv::imwrite(imageIHolder.typeName + ".png", imageIHolder.image);
    }

    imageIHolder.imageRotations = getRotations(imageIHolder.image);
    imageIHolder.moments        = getMoments(imageIHolder.image);
    addContours(imageIHolder.image, imageIHolder.contours);
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
