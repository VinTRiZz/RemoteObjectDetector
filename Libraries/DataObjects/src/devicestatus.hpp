#pragma once

#include <ROD/Error.h>

#include <string>

namespace DataObjects {

/**
 * @brief The DeviceStatus class Status of a server, detector, etc.
 */
struct DeviceStatus : public ErrorUser
{
    struct CommonInfo
    {
        unsigned long   uptime  {};
    };
    CommonInfo common {};

    struct CPUInfo
    {
        double  temperature  {};
        double  loadPercent  {};
    };
    CPUInfo cpu {};

    struct StorageInfo
    {
        unsigned long   spaceTotal      {};
        unsigned long   spaceAvailable  {};
        unsigned long   spaceFree       {};
    };
    StorageInfo storage {};

    std::string toJson();
    bool readJson(const std::string& iString);
};


} // namespace DataObjects
