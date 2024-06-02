#ifndef CAMERAADAPTOR_H
#define CAMERAADAPTOR_H

#include "common.hpp"

namespace Adaptors
{
enum class AdaptorStatus
{
    ERROR,
    BUSY,
    READY
};
    
class CameraAdaptor
{
public:
    // Constructors
    CameraAdaptor(const std::string& deviceFile = "/dev/video0");
    CameraAdaptor(const CameraAdaptor& _oa);
    CameraAdaptor(CameraAdaptor&& _oa);
    ~CameraAdaptor();

    // Operators
    CameraAdaptor& operator=(const CameraAdaptor& _oa);
    CameraAdaptor& operator=(CameraAdaptor&& _oa);
    
    // Get status of driver
    AdaptorStatus status();

    // Ask camera to create a picture
    bool shot(const std::string& outputFile);
    bool shotToBuffer(cv::Mat& imageBuffer);

    // Setup camera to use device
    void setCamera(const std::string& cameraDevicePath);
    
private:
    struct CameraDriverPrivate;
    std::unique_ptr<CameraDriverPrivate> d;
};
    
}

#endif // CAMERAADAPTOR_H
