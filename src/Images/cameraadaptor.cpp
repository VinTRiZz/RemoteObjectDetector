#include "cameraadaptor.hpp"

#include <opencv2/opencv.hpp>
#include "logging.hpp"

struct Adaptors::CameraAdaptor::CameraDriverPrivate
{
    std::string m_deviceFilePath;
    Adaptors::AdaptorStatus m_status;
    cv::VideoCapture m_capture; // OpenCV video capture interface

    CameraDriverPrivate(const std::string devPath) :
        m_deviceFilePath {devPath}
    {

    }
};


Adaptors::CameraAdaptor::CameraAdaptor(const std::string &deviceFile) :
    d {new CameraDriverPrivate(deviceFile)}
{

}

Adaptors::CameraAdaptor::CameraAdaptor(const Adaptors::CameraAdaptor &_oa) :
    d {new CameraDriverPrivate(_oa.d->m_deviceFilePath)}
{
    d->m_capture = _oa.d->m_capture;
    d->m_status = _oa.d->m_status;
}

Adaptors::CameraAdaptor::CameraAdaptor(Adaptors::CameraAdaptor &&_oa) :
    d {new CameraDriverPrivate(std::move(_oa.d->m_deviceFilePath))}
{
    d->m_capture = std::move(_oa.d->m_capture);
    d->m_status = std::move(_oa.d->m_status);
}

Adaptors::CameraAdaptor &Adaptors::CameraAdaptor::operator=(const Adaptors::CameraAdaptor &_oa)
{
    d->m_deviceFilePath = _oa.d->m_deviceFilePath;
    d->m_capture = _oa.d->m_capture;
    d->m_status = _oa.d->m_status;
    return *this;
}

Adaptors::CameraAdaptor &Adaptors::CameraAdaptor::operator=(Adaptors::CameraAdaptor &&_oa)
{
    d->m_deviceFilePath = std::move(_oa.d->m_deviceFilePath);
    d->m_capture = std::move(_oa.d->m_capture);
    d->m_status = std::move(_oa.d->m_status);
    return *this;
}

Adaptors::CameraAdaptor::~CameraAdaptor()
{
    deinit();
}

Adaptors::AdaptorStatus Adaptors::CameraAdaptor::status()
{
    return d->m_status;
}

void Adaptors::CameraAdaptor::init()
{
    d->m_status = Adaptors::AdaptorStatus::BUSY;

    // Try to open file provided into class
    d->m_capture.open(d->m_deviceFilePath);
    if (!d->m_capture.isOpened())
    {
        LOG_OPRES_ERROR("(Camera) Can't open file: [ %s ] Error text: [ %s ]", d->m_deviceFilePath.c_str(), strerror(errno));
        d->m_status = Adaptors::AdaptorStatus::ERROR;
        return;
    }

    LOG_OPRES_SUCCESS("(Camera) Init succeed");
    d->m_status = Adaptors::AdaptorStatus::READY;
}

void Adaptors::CameraAdaptor::deinit()
{
    if (d->m_capture.isOpened())
        d->m_capture.release();
}

bool Adaptors::CameraAdaptor::shot(const std::string &outputFile)
{
    if (d->m_status != Adaptors::AdaptorStatus::READY)
        return false;

    d->m_status = Adaptors::AdaptorStatus::BUSY;
    cv::Mat frame;

    // Get picture
    d->m_capture >> frame;

    // Check if image get succeed
    if (frame.empty())
    {
        d->m_status = Adaptors::AdaptorStatus::READY;
        return false;
    }

    // Save picture to path
    cv::imwrite(outputFile.c_str(), frame);

    d->m_status = Adaptors::AdaptorStatus::READY;
    return true;
}
