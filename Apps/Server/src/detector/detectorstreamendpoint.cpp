#include "detectorstreamendpoint.hpp"

#include <Components/Logger/Logger.h>

DetectorStreamEndpoint::DetectorStreamEndpoint(ServerEventLogger& eventLogger) :
    AbstractEndpoint(eventLogger)
{
    m_streamingServer.setRequestProcessor([this](std::vector<uint8_t>&& receivedData){
        std::string str;
        std::copy(receivedData.begin(), receivedData.end(), std::back_inserter(str));
        COMPLOG_DEBUG("GOT UDP PACKET:", str);
    });
}

DetectorStreamEndpoint::~DetectorStreamEndpoint()
{
    DetectorStreamEndpoint::stop();
}

void DetectorStreamEndpoint::start(uint16_t port)
{
    m_streamingServer.start(port);
}

bool DetectorStreamEndpoint::isWorking() const
{
    return m_streamingServer.isWorking();
}

void DetectorStreamEndpoint::stop()
{
    m_streamingServer.stop();
}
