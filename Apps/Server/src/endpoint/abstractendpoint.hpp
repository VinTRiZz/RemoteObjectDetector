#pragma once

#include "servereventlogger.hpp"
#include <stdint.h>

class AbstractEndpoint
{
public:
    AbstractEndpoint(ServerEventLogger& eventLogger);

    virtual void start(uint16_t port) = 0;
    virtual bool isWorking() const = 0;
    virtual void stop() = 0;

protected:
    ServerEventLogger& m_eventLogger;
};

