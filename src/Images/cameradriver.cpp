#include "cameradriver.h"

#include <opencv2/opencv.hpp>


struct InputProcessing::CameraDriver::CameraDriverPrivate
{
    const std::string m_deviceFilePath;
    InputProcessing::DriverStatus m_status;
    cv::VideoCapture m_capture;

    CameraDriverPrivate(const std::string devPath) :
        m_deviceFilePath {devPath}
    {

    }
};


InputProcessing::CameraDriver::CameraDriver(const std::string &deviceFile) :
    d {new CameraDriverPrivate(deviceFile)}
{

}

InputProcessing::CameraDriver::CameraDriver(const InputProcessing::CameraDriver &od) :
    d {new CameraDriverPrivate(od.d->m_deviceFilePath)}
{
    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
}

InputProcessing::CameraDriver::CameraDriver(InputProcessing::CameraDriver &&od) :
    d {new CameraDriverPrivate(std::move(od.d->m_deviceFilePath))}
{
    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
}

InputProcessing::CameraDriver &InputProcessing::CameraDriver::operator=(const InputProcessing::CameraDriver &od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
    return *this;
}

InputProcessing::CameraDriver &InputProcessing::CameraDriver::operator=(InputProcessing::CameraDriver &&od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
    return *this;
}

InputProcessing::CameraDriver::~CameraDriver()
{
    deinit();

    if (d) delete d;
}

void InputProcessing::CameraDriver::init()
{
    d->m_status = InputProcessing::DriverStatus::BUSY;

    d->m_capture.open(d->m_deviceFilePath);
    if (d->m_capture.isOpened())
    {
        d->m_status = InputProcessing::DriverStatus::ERROR;
        return;
    }
}

void InputProcessing::CameraDriver::deinit()
{
    if (d->m_capture.isOpened())
        d->m_capture.release();
}

bool InputProcessing::CameraDriver::shot(const std::string &outputFile)
{
    if (d->m_status != InputProcessing::DriverStatus::READY)
        return false;

    d->m_status = InputProcessing::DriverStatus::BUSY;
    cv::Mat frame;

    // Get picture
    d->m_capture >> frame;

    // Check if image get succeed
    if (frame.empty())
    {
        d->m_status = InputProcessing::DriverStatus::READY;
        return false;
    }

    d->m_status = InputProcessing::DriverStatus::READY;
}
