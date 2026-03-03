#include "detectoreventendpoint.hpp"

#include <Components/Logger/Logger.h>

DetectorEventEndpoint::DetectorEventEndpoint(ServerEventLogger &eventLogger) :
    AbstractEndpoint(eventLogger)
{
    // TODO: Set processor for events
}

DetectorEventEndpoint::~DetectorEventEndpoint()
{
    DetectorEventEndpoint::stop();
}

void DetectorEventEndpoint::start(uint16_t port)
{
    m_deviceEventServer.listen("0.0.0.0", port);
}

bool DetectorEventEndpoint::isWorking() const
{
    return m_deviceEventServer.isListening();
}

void DetectorEventEndpoint::stop()
{
    m_deviceEventServer.stop();
}
