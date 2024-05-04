#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>

namespace Common
{

cv::Mat loadImage(const std::string& filepath);

}

#endif // COMMON_H
