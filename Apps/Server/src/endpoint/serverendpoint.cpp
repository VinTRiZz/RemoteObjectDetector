#include "serverendpoint.hpp"

#include "../detector/detectorendpoint.hpp"
#include "../management/managementendpoint.hpp"
#include "servereventlogger.hpp"

#include <Components/Logger/Logger.h>

#include <thread>

struct ServerEndpoint::Impl
{
    Database::SQLiteDatabase db;
    ServerEventLogger eventLogger {db};

    Detector::Endpoint  detectorEndpoint {eventLogger};
    std::thread         detectorThread;

    Management::Endpoint    managementEndpoint {eventLogger};
};

ServerEndpoint::ServerEndpoint()
{

}

ServerEndpoint::~ServerEndpoint()
{
    stop();

    // Edge case
    if (d == nullptr) {
        return;
    }
    if (d->detectorThread.joinable()) {
        d->detectorThread.join();
    }
    d->eventLogger.logEvent(ServerCommon::EventType::Stopped);
}

void ServerEndpoint::start(uint16_t detectorGatewayPort, uint16_t managementPort)
{
    d = std::make_unique<Impl>();
    auto res = d->db.setDatabase("./local.db");
    if (!res) {
        COMPLOG_ERROR("DB set failed. Reason:", d->db.getLastError());
        return;
    }

    if (!d->eventLogger.init()) {
        COMPLOG_ERROR("Failed to configure event logging");
        return;
    }
    d->eventLogger.logEvent(ServerCommon::EventType::Started);

    d->detectorThread = std::thread([this, detectorGatewayPort](){
        d->detectorEndpoint.start(detectorGatewayPort);
    });
    d->managementEndpoint.start(managementPort);
}

bool ServerEndpoint::isWorking() const
{
    if (d.get() == nullptr) {
        return false;
    }
    return (d->detectorEndpoint.isWorking() || d->managementEndpoint.isWorking());
}

void ServerEndpoint::stop()
{
    if (!isWorking()) {
        return;
    }
    d->detectorEndpoint.stop();
    d->managementEndpoint.stop();
}
