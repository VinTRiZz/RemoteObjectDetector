#include "serverendpoint.hpp"

#include <ROD/Exchange/Events.h>
#include <ROD/ImageProcessing/Utility.h>

#include <Components/Logger/Logger.h>
#include <Components/Filework/Common.h>
#include <Components/Common/Utils.h>
#include <Components/Common/DirectoryManager.h>

#include "detectoreventendpoint.hpp"
#include "detectorstreamendpoint.hpp"
#include "managementendpoint.hpp"

#include "eventprocessors/servereventprocessor.hpp"
#include "eventprocessors/detectoreventprocessor.hpp"

#include <ROD/Servers/Records.h>

struct ServerEndpoint::Impl
{
    // Common
    Database::RecordManagerPtr recordManager { std::make_shared<Database::RecordManager>("main_server_" + Common::createRandomString(32)) };

    // Processors for events
    std::shared_ptr<ServerEventProcessor>       serverEventProcessor    { std::make_shared<ServerEventProcessor>() };
    std::shared_ptr<DetectorEventProcessor>     detectorEventProcessor  { std::make_shared<DetectorEventProcessor>() };

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
        d->serverEventProcessor->setEventProcessor(evt, [this, eventTypeString = Protocol::toString(evt)](auto&& ev) {
            COMPLOG_INFO("SERVER EVENT:", eventTypeString, ev.getPayload());
        });
    };
    addEmptyLog(Protocol::EventType::ServerStarted);
    addEmptyLog(Protocol::EventType::ServerStopped);
    addEmptyLog(Protocol::EventType::ServerRebootCalled);
    addEmptyLog(Protocol::EventType::ServerShutdownCalled);

    // TODO: Read from config?
    d->recordManager->setDatabase("main");
    d->recordManager->setServer("127.0.0.1", 10001);
    d->recordManager->setUser("server", "serv_auth_password");
    d->recordManager->init();

    // In other threads
    d->detectorEventEndpoint.setEventProcessor(d->detectorEventProcessor);
    d->detectorEventEndpoint.start(wsEventPort);

    // Image stream processor
    d->detectorStreamingEndpoint.setImageReceivedCallback([](auto&& receivedImage) -> void {
        COMPLOG_DEBUG("Received image from:", receivedImage.getSenderId(), "with id:", receivedImage.getId());

        auto& dirManager = Common::DirectoryManager::getInstance();
        auto dataDir = dirManager.getDirectory(Common::DirectoryManager::Data);
        auto saveFile = dataDir / (std::to_string(receivedImage.getId()) + ".png");
        ImageProcessing::Utility::saveImage(receivedImage.getImage(), saveFile);
    });
    d->detectorStreamingEndpoint.start(udpStreamingPort);

    d->managementEndpoint.setRecordManager(d->recordManager);
    d->managementEndpoint.setEventProcessor(d->serverEventProcessor);

    d->serverEventProcessor->addServerEvent(Protocol::EventType::ServerStarted,
                                           std::string("[ API ") + std::to_string(httpAPIPort) +
                                           " EVENT " + std::to_string(wsEventPort) +
                                           " STREAMING " + std::to_string(udpStreamingPort) + " ]");

    // In main thread
    d->managementEndpoint.start(httpAPIPort);

    COMPLOG_INFO("RemoteObjectDetector server exited (SERVERE EVENT!)"); // easter egg (my mistake in event logging)
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
