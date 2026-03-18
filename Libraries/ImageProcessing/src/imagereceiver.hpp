#pragma once

#include <memory>
#include <string>
#include <functional>
#include <opencv2/core.hpp>
#include <oneapi/tbb.h>

#include "utility.hpp"

namespace ImageProcessing {

class ImageReceiver {
public:
    ImageReceiver(oneapi::tbb::task_arena& arena, uint16_t listenPort);
    ~ImageReceiver();

    /**
     * @brief setFrameCallback  Set callback to process received image
     * @param cameraId          Sender of image ID
     * @param callback          Callback to process image
     */
    void setFrameCallback(const std::string& cameraId, std::function<void(cv::Mat&&)> callback);

    /**
     * @brief setCameraConfig   Add camera configuration to setup receiving pipeline
     * @param cameraId          ID of a camera
     * @param conf              Configuration of camera
     */
    void setCameraConfig(const std::string& cameraId, const Utility::CameraPipelineConfig& conf);

    bool start();
    void stop();
    bool isRunning() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace ImageProcessing
