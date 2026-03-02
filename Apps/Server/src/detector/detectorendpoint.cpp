#include "detectorendpoint.hpp"

#include <Components/Network/ServerUDP.h>
#include <Components/Network/ClientUDP.h>

#include <Components/Logger/Logger.h>

#include <ROD/ImageProcessing.h>

namespace Detector
{

struct Endpoint::Impl
{
    UDP::Server streamingServer;
    UDP::Client streamingDataSender;

    ImageProcessing::Processor imgProcessor {std::thread::hardware_concurrency()};
};

Endpoint::Endpoint(ServerEventLogger &eventLogger) :
    AbstractEndpoint(eventLogger),
    m_db {eventLogger.getServerDb()},
    d {new Impl}
{
    d->streamingServer.setRequestProcessor([this](std::vector<uint8_t>&& receivedData){
        std::string str;
        std::copy(receivedData.begin(), receivedData.end(), std::back_inserter(str));
        COMPLOG_DEBUG("GOT UDP PACKET:", str);
    });
}

Endpoint::~Endpoint()
{
    Endpoint::stop();
}

void Endpoint::start(uint16_t port)
{
    d->streamingServer.start(port);
}

bool Endpoint::isWorking() const
{
    return d->streamingServer.isWorking();
}

void Endpoint::stop()
{
    if (!Endpoint::isWorking()) {
        return;
    }
    d->streamingServer.stop();
}

}
