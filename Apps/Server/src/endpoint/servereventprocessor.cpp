#include "servereventprocessor.hpp"

ServerEventProcessor::ServerEventProcessor() :
    EventProcessor()
{
    setProcessorName("Server event processor");
}

void ServerEventProcessor::addServerEvent(Protocol::EventType etype, const std::string &evPayload)
{
    Protocol::Event ev;
    ev.setType(etype);
    ev.setPayload(evPayload);
    addEvent(std::move(ev));
}
