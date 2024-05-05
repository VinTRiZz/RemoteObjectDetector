#include "cameraadaptor.hpp"

#include <opencv2/opencv.hpp>
#include "logging.hpp"

struct Adaptors::CameraAdaptor::CameraDriverPrivate
{
    const std::string m_deviceFilePath;
    Adaptors::AdaptorStatus m_status;
    cv::VideoCapture m_capture;

    CameraDriverPrivate(const std::string devPath) :
        m_deviceFilePath {devPath}
    {

    }
};


Adaptors::CameraAdaptor::CameraAdaptor(const std::string &deviceFile) :
    d {new CameraDriverPrivate(deviceFile)}
{

}

Adaptors::CameraAdaptor::CameraAdaptor(const Adaptors::CameraAdaptor &od) :
    d {new CameraDriverPrivate(od.d->m_deviceFilePath)}
{
    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
}

Adaptors::CameraAdaptor::CameraAdaptor(Adaptors::CameraAdaptor &&od) :
    d {new CameraDriverPrivate(std::move(od.d->m_deviceFilePath))}
{
    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
}

Adaptors::CameraAdaptor &Adaptors::CameraAdaptor::operator=(const Adaptors::CameraAdaptor &od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = od.d->m_capture;
    d->m_status = od.d->m_status;
    return *this;
}

Adaptors::CameraAdaptor &Adaptors::CameraAdaptor::operator=(Adaptors::CameraAdaptor &&od)
{
    if (!d) d = new CameraDriverPrivate(od.d->m_deviceFilePath);

    d->m_capture = std::move(od.d->m_capture);
    d->m_status = std::move(od.d->m_status);
    return *this;
}

Adaptors::CameraAdaptor::~CameraAdaptor()
{
    deinit();

    if (d) delete d;
}

Adaptors::AdaptorStatus Adaptors::CameraAdaptor::status()
{
    return d->m_status;
}

void Adaptors::CameraAdaptor::init()
{
    d->m_status = Adaptors::AdaptorStatus::BUSY;

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

    cv::imwrite(outputFile.c_str(), frame);

    d->m_status = Adaptors::AdaptorStatus::READY;
    return true;
}
