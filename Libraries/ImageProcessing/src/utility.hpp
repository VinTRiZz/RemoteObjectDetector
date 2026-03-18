#pragma once

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace ImageProcessing::Utility
{

cv::Mat generateColorBarImage(int width, int height);

std::vector<uint8_t>    serializeMat(const cv::Mat& mat);
cv::Mat                 deserializeMat(const std::vector<uint8_t>& buffer);


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

} // namespace ImageProcessing
