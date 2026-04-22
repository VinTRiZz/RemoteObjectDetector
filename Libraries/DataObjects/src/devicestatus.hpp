#pragma once

#include <ROD/DataObjects/Error.h>
#include "serializableobject.hpp"

#include <string>

namespace DataObjects {

/**
 * @brief The DeviceStatus class Status of a server, detector, etc.
 */
struct DeviceStatus : public SerializableObject,
                      public ErrorUser
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

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string& iString) override;
};


} // namespace DataObjects
