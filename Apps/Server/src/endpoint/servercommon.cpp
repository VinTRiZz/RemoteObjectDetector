#include "servercommon.hpp"

#include <stdexcept>

namespace ServerCommon
{

std::string toString(EventType ev)
{
    switch (ev)
    {
    case Unknown:
        return "Unknown event";

    case Started:
        return "Started";

    case Stopped:
        return "Stopped";

    case ManagerConnected:
        return "ManagerConnected";

    case ManagerDisconnected:
        return "ManagerDisconnected";

    case DetectorConnected:
        return "DetectorConnected";

    case DetectorDisconnected:
        return "DetectorDisconnected";
    }
    throw std::invalid_argument("Unknown event type");
}

}
