#include "serverendpoint.hpp"

#include "../detector/detectorendpoint.hpp"
#include "../management/managementendpoint.hpp"
#include "servereventlogger.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Filework/Common.h>
#include <Components/Common/Utils.h>

#include <thread>

struct ServerEndpoint::Impl
{
    Database::SQLiteDatabase db;
    ServerEventLogger eventLogger {db};

    Detector::Endpoint  detectorEventEndpoint {eventLogger};
    std::thread         detectorEventThread;

    Management::Endpoint    managementEndpoint {eventLogger};
};

ServerEndpoint::ServerEndpoint(const std::string &dbPath) :
    m_dbPath {dbPath}
{
    if (Filework::Common::copyFile(m_dbPath, m_dbPath + ".backup_" + Common::getCurrentTimestampFormatted())) {
        COMPLOG_INFO("Created backup of database");
    }
}

ServerEndpoint::~ServerEndpoint()
{
    stop();

    // Edge case
    if (d == nullptr) {
        return;
    }
    if (d->detectorEventThread.joinable()) {
        d->detectorEventThread.join();
    }
    d->eventLogger.logEvent(ServerCommon::EventType::Stopped);
}

void ServerEndpoint::start(uint16_t wsEventPort, uint16_t httpAPIPort, uint16_t udpStreamingPort)
{
    COMPLOG_INFO("Starting RemoteObjectDetector server. Port configuration:");
    COMPLOG_INFO("Event:    ", wsEventPort);
    COMPLOG_INFO("API:      ", httpAPIPort);
    COMPLOG_INFO("Streaming:", udpStreamingPort);

    d = std::make_unique<Impl>();
    auto res = d->db.setDatabase(m_dbPath);
    if (!res) {
        COMPLOG_ERROR("DB set failed. Reason:", d->db.getLastError());
        return;
    }

    if (!d->eventLogger.init()) {
        COMPLOG_ERROR("Failed to configure event logging");
        return;
    }
    d->eventLogger.logEvent(ServerCommon::EventType::Started);

    d->detectorEventThread = std::thread([this, wsEventPort](){
        d->detectorEventEndpoint.start(wsEventPort);
    });

    // TODO: Start UDP streamer
    d->managementEndpoint.start(httpAPIPort);

    COMPLOG_INFO("RemoteObjectDetector server exited");
}

bool ServerEndpoint::isWorking() const
{
    if (d.get() == nullptr) {
        return false;
    }
    return (d->detectorEventEndpoint.isWorking() || d->managementEndpoint.isWorking());
}

void ServerEndpoint::stop()
{
    if (!isWorking()) {
        return;
    }
    d->detectorEventEndpoint.stop();
    d->managementEndpoint.stop();
}
