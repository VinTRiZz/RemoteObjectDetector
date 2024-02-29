#ifndef MAIN_PROJECT_INTERFACE_H
#define MAIN_PROJECT_INTERFACE_H

// For smart pointers
#include <memory>

// For uint16_t, int8_t etc.
#include <stdint.h>

#define STATUS_CONTAINS(status_1, status_2) (static_cast<int>(status_1)& static_cast<int>(status_2))

namespace ObjectDetector
{

// Describes status of program
enum class ProgramStatus : uint8_t
{
    ERROR            = 1,
    SUCCESS          = 1 << 1,

    READY            = 1 << 2,
    PROCESSING       = 1 << 3,
    IDLE             = 1 << 4,

    INIT_SUCCESS     = ProgramStatus::READY& ProgramStatus::SUCCESS,
    INIT_ERROR       = ProgramStatus::READY& ProgramStatus::ERROR
};

// Interface to work with main project object
class Interface
{
public:
    Interface();
    ~Interface();

    // Set path to image processor configuration
    void setImageProcessorConfigDir(const std::string& cfgPath);

    // ofPath is path to output file that will contain program output
    void setOutputFile(const std::string& ofPath);
    
    // Init program
    void init();

    // Start program
    void start();

    // Stop program
    void stop();

    // Wait for working loop complete
    void poll();

    // Get status of program
    ProgramStatus status() const;
    
private:
    struct InterfacePrivate;
    std::unique_ptr<InterfacePrivate> d;

    void setStatus(ProgramStatus s);
};

}

#endif // MAIN_PROJECT_INTERFACE_H
