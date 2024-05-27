#include "cameraadaptor.hpp"

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
    setCamera(deviceFile);
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

}

Adaptors::AdaptorStatus Adaptors::CameraAdaptor::status()
{
    return d->m_status;
}

bool Adaptors::CameraAdaptor::shot(const std::string &outputFile)
{
    if (d->m_status != Adaptors::AdaptorStatus::READY)
        return false;

    d->m_status = Adaptors::AdaptorStatus::BUSY;
    cv::Mat frame;

    d->m_capture.open(d->m_deviceFilePath);

    // Get picture
    d->m_capture >> frame;

    d->m_capture.release();

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

bool Adaptors::CameraAdaptor::shotToBuffer(cv::Mat &imageBuffer)
{
    if (d->m_status != Adaptors::AdaptorStatus::READY)
        return false;

    d->m_status = Adaptors::AdaptorStatus::BUSY;

    // Get picture
    d->m_capture.open(d->m_deviceFilePath);
    d->m_capture >> imageBuffer;
    d->m_capture.release();

    // Check if image get succeed
    if (imageBuffer.empty())
    {
        d->m_status = Adaptors::AdaptorStatus::READY;
        return false;
    }

    d->m_status = Adaptors::AdaptorStatus::READY;
    return true;
}

void Adaptors::CameraAdaptor::setCamera(const std::string &cameraDevicePath)
{
    d->m_capture.open(d->m_deviceFilePath);
    if (d->m_capture.isOpened())
    {
        d->m_status = AdaptorStatus::READY;
        d->m_capture.release();
    }
}
