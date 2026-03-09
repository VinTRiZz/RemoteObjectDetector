#pragma once

#include <ROD/Protocol.h>

#include <memory>

class ServerCommandProcessor : public Protocol::EventProcessor
{
public:
    ServerCommandProcessor();
    ~ServerCommandProcessor();
};

