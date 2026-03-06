#include "eventprocessor.hpp"

#include <Components/Logger/Logger.h>

namespace Protocol
{

void EventProcessor::addEvent(Protocol::Event &&ev)
{
    auto processor = m_processors.find(ev.type);
    if (processor == m_processors.end()) {
        COMPLOG_WARNING("Ignored event:", Protocol::toString(ev.type));
        return;
    }
    processor->second(std::move(ev));
}

void EventProcessor::addServerEvent(EventType etype, const std::string &evPayload)
{
    addEvent(Protocol::Event{ "server", etype, std::move(evPayload) });
}

void EventProcessor::setEventProcessor(Protocol::EventType etype, DeviceEventProcessor &&deviceEvent)
{
    m_processors.emplace(etype, std::move(deviceEvent));
}

}
