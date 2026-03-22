#pragma once

#include <QString>

namespace API::Structures
{

/**
 * @brief The ServerStatus class Status of a server, got by HTTP
 */
struct ServerStatus
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
};

}