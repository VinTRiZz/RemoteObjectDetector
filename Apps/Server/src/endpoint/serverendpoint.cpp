#include "serverendpoint.hpp"

#include "servercommon.hpp"

#include "../detector/detectoreventendpoint.hpp"
#include "../detector/detectorstreamendpoint.hpp"
#include "../management/managementendpoint.hpp"
#include "servereventlogger.hpp"

#include "servereventprocessor.hpp"
#include "../detector/detectoreventprocessor.hpp"
#include "../detector/detectorcommandprocessor.hpp"

#include <ROD/Protocol.h>

#include <Components/Logger/Logger.h>
#include <Components/Filework/Common.h>
#include <Components/Common/Utils.h>

#include <thread>

struct ServerEndpoint::Impl
{
    // Common
    Database::SQLiteDatabase db;
    ServerEventLogger eventLogger {db};

    // Processors for events
    std::shared_ptr<ServerEventProcessor>       serverEventProcessor    { std::make_shared<ServerEventProcessor>() };
    std::shared_ptr<DetectorEventProcessor>     detectorEventProcessor  { std::make_shared<DetectorEventProcessor>() };
    std::shared_ptr<DetectorCommandProcessor>   commandEventProcessor   { std::make_shared<DetectorCommandProcessor>() };

    // Endpoints
    DetectorEventEndpoint       detectorEventEndpoint;
    DetectorStreamEndpoint      detectorStreamingEndpoint;
    Management::Endpoint        managementEndpoint;
};

ServerEndpoint::ServerEndpoint(const std::string &dbPath) :
    m_dbPath {dbPath}
{
    auto backupFile = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::DirectoryType::Backup) / (std::string("backup_") + Common::getCurrentTimestampFormatted());
    if (Filework::Common::copyFile(m_dbPath, backupFile)) {
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

    d->serverEventProcessor->addServerEvent(Protocol::EventType::ServerStopped);
}

void ServerEndpoint::start(uint16_t wsEventPort, uint16_t httpAPIPort, uint16_t udpStreamingPort)
{
    COMPLOG_INFO("Starting RemoteObjectDetector server. Port configuration:");
    COMPLOG_INFO("API:      ", httpAPIPort);
    COMPLOG_INFO("Event:    ", wsEventPort);
    COMPLOG_INFO("Streaming:", udpStreamingPort);

    d = std::make_unique<Impl>();

    auto addEmptyLog = [this](Protocol::EventType evt){
        d->serverEventProcessor->setEventProcessor(evt, [this, evt](auto&& ev) {
            d->eventLogger.logEvent(evt, ev.getPayload());
        });
    };
    addEmptyLog(Protocol::EventType::ServerStarted);
    addEmptyLog(Protocol::EventType::ServerStopped);
    addEmptyLog(Protocol::EventType::ServerRebootCalled);
    addEmptyLog(Protocol::EventType::ServerShutdownCalled);

    auto res = d->db.setDatabase(m_dbPath);
    if (!res) {
        COMPLOG_ERROR("DB set failed. Reason:", d->db.getLastError());
        return;
    }

    if (!d->eventLogger.init()) {
        COMPLOG_ERROR("Failed to configure event logging");
        return;
    }

    // In other threads
    d->detectorEventEndpoint.setEventProcessor(d->detectorEventProcessor);
    d->detectorEventEndpoint.start(wsEventPort);

    d->detectorStreamingEndpoint.setEventProcessor(d->detectorEventProcessor);
    d->detectorStreamingEndpoint.start(udpStreamingPort);

    d->managementEndpoint.setEventProcessor(d->serverEventProcessor);

    d->serverEventProcessor->addServerEvent(Protocol::EventType::ServerStarted,
                                           std::string("[ API ") + std::to_string(httpAPIPort) +
                                           " EVENT " + std::to_string(wsEventPort) +
                                           " STREAMING " + std::to_string(udpStreamingPort) + " ]");

    // In main thread
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
