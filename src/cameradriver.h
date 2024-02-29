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
    CameraDriver();
    ~CameraDriver();
    
    // Get status of driver
    DriverStatus status();
    
    // Init class
    void init();
    
    // Returns count of devices
    int detectDevices();
    
    // Set device by number got with detectDevices() function
    void setDevice(int no);
    
    // Ask camera to create a picture
    bool shot();
    
    // Get picture data byte array in .jpg format
    std::shared_ptr<uint8_t *> getPictureBytes();
    
private:
    struct CameraDriverPrivate;
    std::unique_ptr<CameraDriverPrivate> d;
};
    
}

#endif // CAMERADRIVER_H
