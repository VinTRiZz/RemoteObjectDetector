#pragma once

#include <ROD/Protocol.h>

#include <functional>
#include <map>

namespace Protocol
{

using DeviceEventProcessor = std::function<void(Protocol::Event&&)>;

/**
 * @brief The EventProcessor class  Event processor to process any event got
 */
class EventProcessor
{
public:
    void addEvent(Protocol::Event&& ev);
    void addServerEvent(Protocol::EventType etype, const std::string& evPayload = {});
    void setEventProcessor(Protocol::EventType etype, DeviceEventProcessor&& deviceEvent);

private:
    std::map<Protocol::EventType, DeviceEventProcessor> m_processors;
};

}
