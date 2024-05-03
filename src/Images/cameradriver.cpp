#include "cameradriver.hpp"

#include <opencv2/opencv.hpp>
#include "logging.hpp"

struct Drivers::CameraDriver::CameraDriverPrivate
{
    const std::string m_deviceFilePath;
    Drivers::DriverStatus m_status;
    cv::VideoCapture m_capture;

    CameraDriverPrivate(const std::string devPath) :
        m_deviceFilePath {devPath}
    {

    }
};


Drivers::CameraDriver::CameraDriver(const std::string &deviceFile) :
    d {new CameraDriverPrivate(deviceFile)}
{

}

Drivers::CameraDriver::CameraDriver(const Drivers::CameraDriver &od) :
    d {new CameraDriverPrivate(od.d->m_deviceFilePath)}
{
    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
}

Drivers::CameraDriver::CameraDriver(Drivers::CameraDriver &&od) :
    d {new CameraDriverPrivate(std::move(od.d->m_deviceFilePath))}
{
    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
}

Drivers::CameraDriver &Drivers::CameraDriver::operator=(const Drivers::CameraDriver &od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
    return *this;
}

Drivers::CameraDriver &Drivers::CameraDriver::operator=(Drivers::CameraDriver &&od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
    return *this;
}

Drivers::CameraDriver::~CameraDriver()
{
    deinit();

    if (d) delete d;
}

Drivers::DriverStatus Drivers::CameraDriver::status()
{
    return d->m_status;
}

void Drivers::CameraDriver::init()
{
    d->m_status = Drivers::DriverStatus::BUSY;

    d->m_capture.open(d->m_deviceFilePath);
    if (!d->m_capture.isOpened())
    {
        LOG_OPRES_ERROR("(Camera) Can't open file: [ %s ] Error text: [ %s ]", d->m_deviceFilePath.c_str(), strerror(errno));
        d->m_status = Drivers::DriverStatus::ERROR;
        return;
    }
    LOG_OPRES_SUCCESS("(Camera) Init succeed");
    d->m_status = Drivers::DriverStatus::READY;
}

void Drivers::CameraDriver::deinit()
{
    if (d->m_capture.isOpened())
        d->m_capture.release();
}

bool Drivers::CameraDriver::shot(const std::string &outputFile)
{
    if (d->m_status != Drivers::DriverStatus::READY)
        return false;

    d->m_status = Drivers::DriverStatus::BUSY;
    cv::Mat frame;

    // Get picture
    d->m_capture >> frame;

    // Check if image get succeed
    if (frame.empty())
    {
        d->m_status = Drivers::DriverStatus::READY;
        return false;
    }

    d->m_status = Drivers::DriverStatus::READY;
    return true;
}
