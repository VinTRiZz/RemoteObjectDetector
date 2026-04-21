#include "server.hpp"

#include <QTcpSocket>
#include <QEventLoop>
#include <QTimer>

#include <Components/Logger/Logger.h>

#include "common/serverconfiguration.hpp"

#include "serverregistry.hpp"
#include "detector.hpp"

#include "implementation/servermanager.hpp"
#include "implementation/detectorinfomanager.hpp"


namespace Web {

using namespace Implementation;

struct Server::Impl
{
    int64_t serverId {};

    bool                isOperationPending {false}; // For info downloading
    ServerManager       serverInterface;
    DetectorInfoManager detectorInfoInterface;

    ServerConfiguration         configuration;
    std::set<DetectorHandler>   detectors;
    bool                        cache_isServerAvailable {false};

    bool checkPort(const QString &host, quint16 port) const {
        QTcpSocket socket;
        socket.connectToHost(host, port);
        if (socket.waitForConnected(1000)) {
            socket.disconnectFromHost();
            return true;
        }
        return false;
    }

    void updateServerAddress() {
        auto serverAddr = configuration.getHost() + ":" + QString::number(configuration.getPort());
        serverInterface.setServer(serverAddr);
        detectorInfoInterface.setServer(serverAddr);
    }
};

Server::Server(int64_t serverId, ServerRegistry *parent)
    : QObject{parent},
    d {new Impl()}
{
    d->serverId = serverId;

    connect(&d->serverInterface, &ServerManager::responseStatus,
            this, [this](bool isOk, auto& serverStatus) {
        if (isOk) {
            emit gotStatus(serverStatus);
        } else {
            emit gotStatus({});
        }
    });
}

Server::~Server()
{
    // TODO: Disconnect if need
}

int64_t Server::getId() const
{
    return d->serverId;
}

void Server::ping()
{
    QMetaObject::invokeMethod(this, [this](){
        if (d->checkPort(d->configuration.getHost(), d->configuration.getPort())) {
            d->cache_isServerAvailable = true;
            emit serverIsAvailable();
        } else {
            d->cache_isServerAvailable = false;
            emit serverIsUnavailable();
        }
    });
}

bool Server::isServerAvailable() const
{
    return d->cache_isServerAvailable;
}

void Server::setHost(const QString &hostname)
{
    d->configuration.setHost(hostname);
    emit configurationChanged();
    d->updateServerAddress();
}

void Server::setPort(const uint16_t &port)
{
    d->configuration.setPort(port);
    emit configurationChanged();
    d->updateServerAddress();
}

void Server::setName(const QString &name)
{
    d->configuration.setName(name);
    emit configurationChanged();
}

void Server::requestPoweroff() const
{
    d->serverInterface.requestPoweroff();
}

void Server::requestReboot() const
{
    d->serverInterface.requestReboot();
}

void Server::requestStatus() const
{
    d->serverInterface.requestStatus();
}

const ServerConfiguration &Server::getConfiguration() const
{
    return d->configuration;
}

bool Server::addDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto pDetector = new Detector(this);
    pDetector->setConfiguration(conf);

    // TODO: Register detector in remote

    DetectorHandler det(pDetector);
    d->detectors.insert(det);
    emit detectorAdded(det);
    return true;
}

void Server::removeDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto targetIt = std::find_if(d->detectors.begin(), d->detectors.end(), [&conf](const auto& detectorHdl){
        return (detectorHdl->getConfiguration() == conf);
    });
    if (targetIt == d->detectors.end()) {
        return;
    }

    // TODO: Remove detector in remote

    auto detHdl = *targetIt;
    emit detectorAboutToRemove(detHdl);
    d->detectors.erase(targetIt);

    // Delete safely
    auto pDet = detHdl.get();
    detHdl.invalidate();
    pDet->deleteLater();
}

std::set<DetectorHandler> Server::getDetectors() const
{
    QEventLoop loop;


    connect(&d->detectorInfoInterface, &DetectorInfoManager::responseDetectorInfoList,
            &loop, [&loop, this](bool isOk, const auto& infos) {
        for (auto& info : infos) {
            auto pDetector = new Detector(const_cast<Server*>(this)); // wtf? const is not acceptable.
            pDetector->setConfiguration(info);

            DetectorHandler det(pDetector);
            d->detectors.insert(det);
        }
        loop.quit();
    });

    // Request timeout
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);

    d->detectorInfoInterface.requestDetectorInfoList();

    loop.exec();
    return d->detectors;
}

bool Server::operator<(const Server &s) const
{
    return d->configuration < s.d->configuration;
}

void Server::setConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;
    d->updateServerAddress();
}

} // namespace Web
