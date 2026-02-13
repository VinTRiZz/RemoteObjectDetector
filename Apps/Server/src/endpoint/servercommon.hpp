#pragma once

#include <string>

namespace ServerCommon
{

enum EventType : int
{
    Unknown = -1,

    // Server common
    Started,
    Stopped,

    // Manager
    ManagerConnected,
    ManagerDisconnected,

    // Detector
    DetectorConnected,
    DetectorDisconnected,
};

std::string toString(EventType ev);

}
