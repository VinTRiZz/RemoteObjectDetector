#ifndef COMMON_H
#define COMMON_H

// OpenCV header
#include <opencv2/opencv.hpp>

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

// Data containers
#include <string>
#include <list>
#include <map>
#include <vector>

// Logging
#include "../logging.hpp"

// STD algorithms
#include <algorithm>

// Directory processing
#include <dirent.h>

// Compare speed increasing
#include <thread>
#include <mutex>


namespace Common
{

// Size constants of object setup rect
const uint64_t CAMERA_CENTER_RECT_X         = 640 / 3;
const uint64_t CAMERA_CENTER_RECT_Y         = 480 / 3;
const uint64_t CAMERA_CENTER_RECT_HEIGHT    = 640 * 2 / 3;
const uint64_t CAMERA_CENTER_RECT_WIDTH     = 480 * 2 / 3;

// Types to work easier
typedef std::vector<cv::Point>  ContourType;
typedef std::vector<double>     MomentsType;

// Load image using file path, return null image if error occurs
cv::Mat loadImage(const std::string& filepath);

// Get moments of image
std::vector<double> createMoments(const cv::Mat& image);


// Describes what method to use for image processing
enum class CompareMethod :uint8_t
{
    COMPARE_METHOD_NONE, // Used for invalid objects found on image
    COMPARE_METHOD_TEMPLATE,
    COMPARE_METHOD_HISTOGRAM,
    COMPARE_METHOD_MOMENTS,
};

// Search for objects on an image and return vector if them
std::vector<cv::Mat> getObjects(const cv::Mat& targetImage, cv::Ptr<cv::BackgroundSubtractor> &pBacgroundSub);



struct TypeInfoHolder
{
    // Basic info
    std::string typeName;
    std::string imagePath;
    cv::Mat image;

    // Image describing info
    std::vector<cv::Mat>        imageRotations;
    std::vector<ContourType>    contours;
    std::vector<MomentsType>    moments;
    std::list<cv::Mat>          histograms;

    bool operator ==(const TypeInfoHolder& typeIHolder) { return (typeIHolder.typeName == this->typeName); }
};

// Load objects from directory into typeList
void loadObjects(const std::string& path, std::list<Common::TypeInfoHolder>& typeList, cv::Ptr<cv::BackgroundSubtractor>& pBackgroundSub);

}

#endif // COMMON_H
