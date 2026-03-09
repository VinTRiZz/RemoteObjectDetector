#include "eventprocessor.hpp"

#include <Components/Logger/Logger.h>

namespace Protocol
{

void EventProcessor::addEvent(Protocol::Event &&ev)
{
    auto processor = m_processors.find(ev.getType());
    if (processor == m_processors.end()) {
        COMPLOG_WARNING("PROCESSOR: [", m_name, "] Ignored event:", Protocol::toString(ev.getType()));
        return;
    }
    processor->second(std::move(ev));
}

void EventProcessor::setEventProcessor(Protocol::EventType etype, DeviceEventProcessor &&deviceEvent)
{
    m_processors.emplace(etype, std::move(deviceEvent));
}

void EventProcessor::setProcessorName(const std::string &name)
{
    m_name = name;
}

std::string EventProcessor::getName() const
{
    return m_name;
}

}
