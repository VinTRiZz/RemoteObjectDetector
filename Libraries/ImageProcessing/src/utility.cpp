#include "utility.hpp"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <sstream>
#include <opencv2/imgcodecs.hpp>

namespace ImageProcessing::Utility
{

std::vector<uint8_t> generateTestImageBytes(int width, int height)
{
    auto testImage = generateColorBarImage(width, height);
    return serializeMat(testImage);
}

cv::Mat generateColorBarImage(int width, int height) {
    cv::Mat img(height, width, CV_8UC3);
    int barWidth = width / 8;
    std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 255, 255), // white
        cv::Scalar(255, 255, 0),   // yellow
        cv::Scalar(0, 255, 255),   // cyan
        cv::Scalar(0, 255, 0),     // green
        cv::Scalar(255, 0, 255),   // magenta
        cv::Scalar(255, 0, 0),     // red
        cv::Scalar(0, 0, 255),     // blue
        cv::Scalar(0, 0, 0)        // black
    };
    for (int i = 0; i < 8; ++i) {
        cv::Rect roi(i * barWidth, 0, barWidth, height);
        img(roi) = colors[i];
    }
    return img;
}

std::vector<uint8_t> serializeMat(const cv::Mat& mat) {
    std::vector<uint8_t> buffer;
    cv::imencode(".jpg", mat, buffer);
    return buffer;
}

cv::Mat deserializeMat(const std::vector<uint8_t>& buffer) {
    cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_COLOR);
    return mat;
}

std::string createSenderPipeline(const CameraPipelineConfig &config) {
    std::ostringstream pipeline;

    pipeline << "appsrc name=source ! "

                // Image config
             << "video/x-raw,format=BGR,"
             << "width=" << config.frameWidth << ",height=" << config.frameHeight
             << ",framerate=" << config.fps << "/1 ! "

                // Auto video convert
             << "videoconvert ! "

                // Network configuration
             << "x264enc bitrate=" << config.bitrate
             << " tune=zerolatency speed-preset=ultrafast ! "
             << "h264parse ! rtph264pay config-interval=1 pt=96 ! "

                // Target server info
             << "rtspclientsink location=rtsp://" << config.serverIp << ":" << config.serverPort << "/" << config.cameraId
             << " sync=false async=false";

    return pipeline.str();
}

std::string createReceiverPipeline(const CameraPipelineConfig &config) {
    std::ostringstream pipeline;

    pipeline << "rtspsrc location=rtsp://" << config.serverIp << ":"
             << config.serverPort << "/" << config.cameraId
             << " latency=0 ! "
             << "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! "
             << "video/x-raw,format=BGR ! appsink name=sink sync=false";

    return pipeline.str();


    // // Example pipeline for receiving RTSP stream
    // std::ostringstream oss;
    // oss << "uridecodebin uri=rtsp://" << config.serverIp << ":" << config.serverPort
    //     << "/?camera_id=" << config.cameraId << " ! videoconvert ! appsink name=appsink";
    // return oss.str();
}

}
