#include "servereventprocessor.hpp"

#include <thread>

#include <Components/Logger/Logger.h>

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

void ServerEventProcessor::startReboot()
{
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // For sending
#ifdef DEBUG_BUILD_MODE
    COMPLOG_DEBUG("REBOT PROCEDURE CALLED");
#else
    system("systemctl reboot");
#endif // DEBUG_BUILD_MODE
}

void ServerEventProcessor::startPoweroff()
{
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // For sending
#ifdef DEBUG_BUILD_MODE
    COMPLOG_DEBUG("POWEROFF PROCEDURE CALLED");
#else
    system("systemctl poweroff");
#endif // DEBUG_BUILD_MODE
}
