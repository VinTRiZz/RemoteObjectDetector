#include "events.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace Protocol
{

Event::Event(const std::string &initialTxt)
{
    readRaw(initialTxt);
}

bool Event::readRaw(const std::string &txt) noexcept
{
    try {
        auto eventRequest = nlohmann::json::parse(txt);

        for (auto& [key, value] : eventRequest["headers"].items()) {
            m_headers[key] = value;
        }
        m_type = eventRequest["event"];
        m_payload = eventRequest["payload"];

        return true;
    } catch (nlohmann::json::exception& parseEx) {
        COMPLOG_ERROR("Event parse exception:", parseEx.what());
        return false;
    } catch (...) {
        return false;
    }
}

std::string Event::toRaw() const
{
    nlohmann::json res;

    auto& resH = res["headers"];
    for (auto& [key, value] : m_headers) {
        resH[key] = value;
    }
    res["event"] = m_type;
    res["payload"] = m_payload;

    return res.dump();
}

void Event::setHeader(const std::string &header, const std::string &value)
{
    m_headers[header] = value;
}

std::string Event::getHeader(const std::string &headerName) const
{
    auto targetHeader = m_headers.find(headerName);
    if (targetHeader == m_headers.end()) {
        return {};
    }
    return targetHeader->second;
}

void Event::setType(EventType etype)
{
    m_type = etype;
}

EventType Event::getType() const noexcept
{
    return m_type;
}

void Event::setPayload(const std::string &iData)
{
    m_payload = iData;
}

void Event::setPayload(std::string &&iData)
{
    m_payload = std::move(iData);
}

std::string Event::getPayload() const
{
    return m_payload;
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
    case DetectorSoftVersionAdded:
        return "DetectorSoftVersionAdded";
    case DetectorSoftVersionChanged:
        return "DetectorSoftVersionChanged";
    case DetectorSoftVersionRemoved:
        return "DetectorSoftVersionRemoved";

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
