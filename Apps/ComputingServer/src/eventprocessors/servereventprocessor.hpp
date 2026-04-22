#pragma once

#include <ROD/Exchange/Events.h>
#include <ROD/Servers/EventProcessor.h>

class ServerEventProcessor : public Protocol::EventProcessor
{
public:
    ServerEventProcessor();

    void addServerEvent(Protocol::EventType etype, const std::string& evPayload = {});

    void startReboot();
    void startPoweroff();
};

