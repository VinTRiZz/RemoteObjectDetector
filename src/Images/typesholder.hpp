#ifndef TYPESHOLDER_HPP
#define TYPESHOLDER_HPP

#include "common.hpp"

// Types to work easier
typedef std::vector<cv::Point>  ContourType;
typedef std::vector<double>     MomentsType;

struct TypeInfoHolder
{
    // Basic info
    std::string typeName;
    std::string imagePath;
    cv::Mat image;

    // Image describing info
    std::vector<cv::Mat>        imageRotations;
    std::vector<ContourType>    contours;
    MomentsType                 huMoments;
    std::list<cv::Mat>          histograms;

    bool operator ==(const TypeInfoHolder& typeIHolder) { return (typeIHolder.typeName == this->typeName); }
};

struct FoundObjects
{
    std::vector<std::pair<std::string, double> > percents;
};

class TypesHolder
{
public:
    TypesHolder(cv::Ptr<cv::BackgroundSubtractor>& pBSub);

    cv::Ptr<cv::BackgroundSubtractor>& m_pBackSub;
    std::list<TypeInfoHolder> typeList;

    // Size constants of object setup rect
    static const uint64_t CAMERA_CENTER_RECT_X         = 640 / 3;
    static const uint64_t CAMERA_CENTER_RECT_Y         = 480 / 3;
    static const uint64_t CAMERA_CENTER_RECT_HEIGHT    = 640 * 2 / 3;
    static const uint64_t CAMERA_CENTER_RECT_WIDTH     = 480 * 2 / 3;


    // Load image using file path, return null image if error occurs
    cv::Mat loadImage(const std::string& filepath);

    // Get moments of image
    MomentsType createHuMoments(const cv::Mat& image);
    std::vector<MomentsType > createHuMomentsSet(const cv::Mat &image);

    // Search for objects on an image and return vector if them
    std::vector<cv::Mat> getObjects(const cv::Mat& targetImage);

    bool addType(TypeInfoHolder &imageIHolder);
    void setupInfoHolder(TypeInfoHolder &imageIHolder);

    void createRotationSet(std::vector<cv::Mat>& result, size_t &currentIndex);
    std::list<cv::Mat> createHistograms(const std::vector<cv::Mat> &imageRotations);

    void addContours(const cv::Mat &img, std::vector<ContourType> &imageContours);
    std::vector<cv::Mat> createRotations(const cv::Mat& image);
};

#endif // TYPESHOLDER_HPP
