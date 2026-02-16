#pragma once

#include <string>
#include <Components/Common/DirectoryManager.h>

namespace ServerCommon
{

const int DIRTYPE_SOFT_VERSIONS = Common::DirectoryManager::DirectoryType::UserDefined + 1;

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
