#pragma once

#include <ROD/Exchange/Events.h>
#include <ROD/Servers/EventProcessor.h>

#include <memory>

class ServerCommandProcessor : public Protocol::EventProcessor
{
public:
    ServerCommandProcessor();
    ~ServerCommandProcessor();
};

