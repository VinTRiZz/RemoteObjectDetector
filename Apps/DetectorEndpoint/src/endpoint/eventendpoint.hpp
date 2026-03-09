#pragma once

#include <stdint.h>
#include "servercommandprocessor.hpp"

class EventEndpoint
{
public:
    EventEndpoint();
    ~EventEndpoint();

    void setToken(const std::string& token);

    void connect(const std::string& serverHost, uint16_t eventPort);
    bool isConnected() const;
    void disconnect();

    ServerCommandProcessor& getEventProcessor();

private:
    std::string m_token;
    ServerCommandProcessor m_eventProcessor;

    struct Impl;
    std::unique_ptr<Impl> d;
};
