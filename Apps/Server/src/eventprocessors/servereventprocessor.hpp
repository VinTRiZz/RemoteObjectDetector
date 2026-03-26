#pragma once

#include <ROD/Protocol.h>

class ServerEventProcessor : public Protocol::EventProcessor
{
public:
    ServerEventProcessor();

    void addServerEvent(Protocol::EventType etype, const std::string& evPayload = {});

    void startReboot();
    void startPoweroff();
};

