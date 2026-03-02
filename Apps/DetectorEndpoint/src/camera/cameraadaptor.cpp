#include "cameraadaptor.hpp"

#include <thread>
#include <atomic>

struct Adaptors::CameraAdaptor::Impl
{
    std::string m_deviceFilePath;
    std::atomic<Adaptors::AdaptorStatus> m_status;
    cv::VideoCapture m_capture; // OpenCV video capture interface
};


Adaptors::CameraAdaptor::CameraAdaptor(const std::string &deviceFile) :
    d {new Impl}
{
    setCameraDevice(deviceFile);
}

Adaptors::CameraAdaptor::~CameraAdaptor()
{

}

Adaptors::AdaptorStatus Adaptors::CameraAdaptor::status()
{
    return d->m_status.load(std::memory_order_acquire);
}

bool Adaptors::CameraAdaptor::shot(const std::string &outputFile)
{
    // Ожидание возможности считывания данных
    for (unsigned curMs = 0; d->m_status.load(std::memory_order_acquire) != Adaptors::AdaptorStatus::READY && curMs < 1000; ++curMs) {
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (d->m_status.load(std::memory_order_acquire) != Adaptors::AdaptorStatus::READY) {
        return false;
    }

    d->m_status.store(Adaptors::AdaptorStatus::BUSY, std::memory_order_release);
    cv::Mat frame;

    d->m_capture.open(d->m_deviceFilePath);

    // Get picture
    d->m_capture >> frame;

    d->m_capture.release();

    // Check if image get succeed
    if (frame.empty()) {
        d->m_status.store(Adaptors::AdaptorStatus::READY, std::memory_order_release);
        return false;
    }

    // Save picture to path
    cv::imwrite(outputFile.c_str(), frame);

    d->m_status.store(Adaptors::AdaptorStatus::READY, std::memory_order_release);
    return true;
}

bool Adaptors::CameraAdaptor::shotToBuffer(cv::Mat &imageBuffer)
{
    if (d->m_status.load(std::memory_order_acquire) != Adaptors::AdaptorStatus::READY)
        return false;

    d->m_status.store(Adaptors::AdaptorStatus::BUSY, std::memory_order_release);

    // Get picture
    d->m_capture.open(d->m_deviceFilePath);
    d->m_capture >> imageBuffer;
    d->m_capture.release();

    // Check if image get succeed
    if (imageBuffer.empty()) {
        d->m_status.store(Adaptors::AdaptorStatus::READY, std::memory_order_release);
        return false;
    }

    d->m_status.store(Adaptors::AdaptorStatus::READY, std::memory_order_release);
    return true;
}

void Adaptors::CameraAdaptor::setCameraDevice(const std::string &cameraDevicePath)
{
    d->m_capture.open(d->m_deviceFilePath);
    if (d->m_capture.isOpened()) {
        d->m_status.store(AdaptorStatus::READY, std::memory_order_release);
        d->m_capture.release();
    }
}

std::string Adaptors::CameraAdaptor::getCameraDevice() const
{
    return d->m_deviceFilePath;
}
