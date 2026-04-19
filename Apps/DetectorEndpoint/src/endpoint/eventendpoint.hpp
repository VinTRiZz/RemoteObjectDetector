#pragma once

#include <stdint.h>
#include "servercommandprocessor.hpp"

class EventEndpoint
{
public:
    EventEndpoint();
    ~EventEndpoint();

    void setDeviceId(long long devId);

    void setServer(const std::string& serverHost, uint16_t eventPort);

    void connect();
    bool isConnected() const;
    void disconnect();

    ServerCommandProcessor& getEventProcessor();

private:
    long long m_deviceId {};
    ServerCommandProcessor m_eventProcessor;

    struct Impl;
    std::unique_ptr<Impl> d;
};
