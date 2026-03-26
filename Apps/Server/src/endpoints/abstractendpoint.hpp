#pragma once

#include <stdint.h>
#include <ROD/Protocol.h>

class AbstractEndpoint
{
public:
    virtual void start(uint16_t port) = 0;
    virtual bool isWorking() const = 0;
    virtual void stop() = 0;

    virtual void setEventProcessor(const Protocol::EventProcessorPtr& pEventProcessor);

protected:
    Protocol::EventProcessorPtr m_pEventProcessor;
};

