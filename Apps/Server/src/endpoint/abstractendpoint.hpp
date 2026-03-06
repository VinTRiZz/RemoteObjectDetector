#pragma once

#include <stdint.h>
#include <ROD/Protocol.h>

class AbstractEndpoint
{
public:
    AbstractEndpoint(Protocol::EventProcessor& eventProcessor);

    virtual void start(uint16_t port) = 0;
    virtual bool isWorking() const = 0;
    virtual void stop() = 0;

protected:
    Protocol::EventProcessor& m_eventProcessor;
};

