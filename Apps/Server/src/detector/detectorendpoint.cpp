#include "detectorendpoint.hpp"

namespace Detector
{

Endpoint::Endpoint(ServerEventLogger &eventLogger) :
    AbstractEndpoint(eventLogger),
    m_db {eventLogger.getServerDb()}
{

}

Endpoint::~Endpoint()
{
    Endpoint::stop();
}

void Endpoint::start(uint16_t port)
{

}

bool Endpoint::isWorking() const
{
    return false;
}

void Endpoint::stop()
{
    if (!Endpoint::isWorking()) {
        return;
    }
}

}
