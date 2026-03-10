#pragma once

#include <opencv2/opencv.hpp>

namespace ImageProcessing::Utility
{

cv::Mat generateColorBarImage(int width, int height);

std::vector<uint8_t>    serializeMat(const cv::Mat& mat);
cv::Mat                 deserializeMat(const std::vector<uint8_t>& buffer);

}
