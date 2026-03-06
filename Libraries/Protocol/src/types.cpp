#include "types.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace Protocol
{

bool Event::readRaw(const std::string &txt) noexcept
{
    try {
        auto eventRequest = nlohmann::json::parse(txt);

        device = eventRequest["dev"];
        type = eventRequest["event"];
        payload = eventRequest["payload"];

        return true;
    } catch (nlohmann::json::exception& parseEx) {
        COMPLOG_ERROR("Event parse exception:", parseEx.what());
        return false;
    } catch (...) {
        return false;
    }
}

Event Event::fromRaw(const std::string &txt)
{
    Event res;
    res.readRaw(txt);
    return res;
}

std::string Event::toRaw() const
{
    nlohmann::json res;

    res["dev"] = device;
    res["event"] = type;
    res["payload"] = payload;

    return res.dump();
}

std::string toString(EventType etype)
{
    switch (etype)
    {
    case Undefined:
        return "UNDEFINED_EVENT";

    // Server event
    case ServerStarted:
        return "ServerStarted";
    case ServerStopped:
        return "ServerStopped";
    case ServerShutdownCalled:
        return "ServerShutdownCalled";
    case ServerRebootCalled:
        return "ServerRebootCalled";
    case ServerAlert:
        return "ServerAlert";

    // Detector software
    case VersionAdded:
        return "VersionAdded";
    case VersionChanged:
        return "VersionChanged";
    case VersionRemoved:
        return "VersionRemoved";

    // Regular
    case DetectorConnected:
        return "DetectorConnected";
    case DetectorDisconnected:
        return "DetectorDisconnected";
    case DetectorAlert:
        return "DetectorAlert";

    // Detection
    case DetectedObject:
        return "DetectedObject";
    case FailedObjectDetection:
        return "FailedObjectDetection";
    }
    throw std::invalid_argument(std::string("Unknown event type: ") + std::to_string(etype));
}

}
