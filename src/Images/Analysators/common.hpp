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
#include "logging.hpp"

// STD algorithms
#include <algorithm>

// Directory processing
#include <dirent.h>

// Compare speed increasing
#include <thread>
#include <mutex>


namespace Common
{

// Contours to use in OpenCV
typedef std::vector<std::vector<cv::Point>> ContoursType;

// Load image using file path, return null image if error occurs
cv::Mat loadImage(const std::string& filepath);

// Add found on image contours to vector, if no any, do nothing
void addContours(cv::Mat &img, ContoursType& imageContours);

// Save cropped from img using foundObjectContours images with baseName as begin of path
void drawFound(cv::Mat& img, ContoursType& foundObjectContours, const std::string& baseName);

// Search for objects on an image and return vector if them
std::vector<cv::Mat> getObjects(const std::string& imageFullPath);

}

#endif // COMMON_H
