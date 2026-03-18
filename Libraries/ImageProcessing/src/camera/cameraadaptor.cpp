#include "cameraadaptor.hpp"

#include "utility.hpp"

#include <opencv2/opencv.hpp>

#include <thread>
#include <atomic>

namespace ImageProcessing
{

struct CameraAdaptor::Impl
{
    std::string deviceFilePath;
    std::atomic<AdaptorStatus> status {AdaptorStatus::ERROR};

    // CV
    cv::VideoCapture shotCamera;    // OpenCV picture capture interface
    cv::VideoWriter  streamer;      // OpenCV video streaming interface

    // GStreamer configuration
    std::string gstPipelineBase {
        "videoconvert ! x264enc tune=zerolatency ! rtph264pay pt=96"
    };
    std::string gstPipeline;

    bool canWork() const {
        return (status.load(std::memory_order_acquire) == AdaptorStatus::READY);
    }

    cv::Mat shot() {
        for (unsigned curMs = 0; !canWork() && curMs < 1000; ++curMs) {
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (!canWork()) return {};
        status.store(AdaptorStatus::BUSY, std::memory_order_release);

        // Get picture
        cv::Mat imageBuffer;
        shotCamera.open(deviceFilePath);
        shotCamera >> imageBuffer;
        shotCamera.release();
        status.store(AdaptorStatus::READY, std::memory_order_release);

        return imageBuffer;
    }
};


CameraAdaptor::CameraAdaptor(const std::string &deviceFile) :
    d {new Impl}
{
    setCameraDevice(deviceFile);
}

CameraAdaptor::~CameraAdaptor()
{
    deinitStreaming();
}

AdaptorStatus CameraAdaptor::getStatus() const
{
    return d->status.load(std::memory_order_acquire);
}

bool CameraAdaptor::shot(const std::string &outputFile)
{
    auto img = d->shot();
    if (img.empty()) {
        return false;
    }
    cv::imwrite(outputFile.c_str(), img);
    setStatus(AdaptorStatus::READY);
    return true;
}

std::vector<uint8_t> CameraAdaptor::shot()
{
    auto img = d->shot();
    if (img.empty()) {
        return {};
    }
    return Utility::serializeMat(img);
}

bool CameraAdaptor::initStreaming(const std::string &configuration)
{
    if (!canWork()) {
        return false;
    }
    d->gstPipeline = configuration;
    d->streamer.open(d->gstPipeline, cv::CAP_GSTREAMER, 0, cv::Size(640, 480), true);
    if (!d->streamer.isOpened()) {
        return false;
    }
    return true;
}

bool CameraAdaptor::initStreamingSimple(const std::string serverIp, uint16_t serverPort)
{
    return initStreaming("udpsink host=" + serverIp + " port=" + std::to_string(serverPort) + " sync=false ! " + d->gstPipelineBase);
}

bool CameraAdaptor::streamShot()
{
    if (!canWork()) {
        return false;
    }
    d->streamer.write(shot());
    return true;
}

void CameraAdaptor::deinitStreaming()
{
    if (!canWork() || !d->streamer.isOpened()) {
        return;
    }
    d->streamer.release();
}

void CameraAdaptor::setCameraDevice(const std::string &cameraDevicePath)
{
    d->shotCamera.open(cameraDevicePath);
    if (!d->shotCamera.isOpened()) {
        return;
    }

    d->deviceFilePath = cameraDevicePath;
    setStatus(AdaptorStatus::READY);
    d->shotCamera.release();
}

std::string CameraAdaptor::getCameraDevice() const
{
    return d->deviceFilePath;
}

bool CameraAdaptor::canWork() const
{
    return getStatus() == AdaptorStatus::READY;
}

void CameraAdaptor::setStatus(AdaptorStatus ast)
{
    d->status.store(ast, std::memory_order_release);
}

}
