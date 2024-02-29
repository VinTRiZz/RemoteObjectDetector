#ifndef CAMERADRIVER_H
#define CAMERADRIVER_H

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

namespace InputProcessing
{
enum class DriverStatus
{
    ERROR,
    BUSY,
    READY
};
    
class CameraDriver
{
public:
    CameraDriver(const std::string& deviceFile = "/dev/video1");
    ~CameraDriver();
    
    // Get status of driver
    DriverStatus status();
    
    // Init class using camera choosen
    void init();

    // Deinit class, releasing camera choosen
    void deinit();
    
    // Ask camera to create a picture
    bool shot(const std::string& outputFile);
    
private:
    struct CameraDriverPrivate;
    std::unique_ptr<CameraDriverPrivate> d;
};
    
}

#endif // CAMERADRIVER_H
