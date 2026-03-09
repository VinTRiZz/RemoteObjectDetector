#pragma once

#include <ROD/Protocol.h>

#include <functional>
#include <map>
#include <memory>

namespace Protocol
{

using DeviceEventProcessor = std::function<void(Protocol::Event&&)>;
class EventProcessor;
using EventProcessorPtr = std::shared_ptr<EventProcessor>;

/**
 * @brief The EventProcessor class  Event processor to process any event got
 */
class EventProcessor
{
public:
    virtual void addEvent(Protocol::Event&& ev);
    void setEventProcessor(Protocol::EventType etype, DeviceEventProcessor&& deviceEvent);

    /**
     * @brief setProcessorName  Set name for debug needs
     * @param name
     */
    void setProcessorName(const std::string& name);
    std::string getName() const;

private:
    std::string m_name;

    std::map<Protocol::EventType, DeviceEventProcessor> m_processors;
};

}
