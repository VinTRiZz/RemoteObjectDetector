#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>

namespace Common
{

typedef std::vector<std::vector<cv::Point>> ContoursType;

cv::Mat loadImage(const std::string& filepath);

void addContours(cv::Mat &img, ContoursType& imageContours);

void drawFound(cv::Mat& img, ContoursType& foundObjectContours, const std::string& baseName);

}

#endif // COMMON_H
