#ifndef CAMERAADAPTOR_H
#define CAMERAADAPTOR_H

// For stable types, such as uint8_t
#include <stdint.h>

// For d-pointer
#include <memory>

// Data containers
#include <string>

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
    
    // Init class using camera choosen
    void init();

    // Deinit class, releasing camera choosen
    void deinit();
    
    // Ask camera to create a picture
    bool shot(const std::string& outputFile);
    
private:
    struct CameraDriverPrivate;
    std::unique_ptr<CameraDriverPrivate> d {nullptr};
};
    
}

#endif // CAMERAADAPTOR_H
