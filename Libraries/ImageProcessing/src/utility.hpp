#pragma once

#include <string>
#include <vector>

#include "common.hpp"

namespace cv {
class Mat;
}

namespace ImageProcessing::Utility
{

cv::Mat generateColorBarImage(int width, int height);
ImageData_t generateTestImageBytes(int width, int height);

ImageData_t serializeMat(const cv::Mat& mat);
cv::Mat     deserializeMat(const ImageData_t& buffer);

bool saveImage(const ImageData_t& img, const std::string& filePath);


/**
 * @brief The CameraPipelineConfig class Configuration of camera to build GStreamer pipeline string
 */
struct CameraPipelineConfig {
    std::string cameraId;
    std::string serverIp;
    uint16_t    serverPort;
    int         frameWidth;
    int         frameHeight;
    double      fps = 30.0;
    int         bitrate = 1024;     // Bitrate in kb/s
    bool        zeroLatency = true; // Latency config in GStreamer
};
std::string createSenderPipeline(const CameraPipelineConfig& config);
std::string createReceiverPipeline(const CameraPipelineConfig& config);

std::vector<uint8_t> calculateImageHash(const ImageData_t& data);

} // namespace ImageProcessing
