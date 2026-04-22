#pragma once

#include <ROD/Exchange/Events.h>
#include <ROD/Servers/EventProcessor.h>

class DetectorEventProcessor : public Protocol::EventProcessor
{
public:
    DetectorEventProcessor();
};

