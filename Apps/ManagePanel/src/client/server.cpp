#include "server.hpp"

#include <QTcpSocket>

#include "common/serverconfiguration.hpp"

#include "serverregistry.hpp"
#include "detector.hpp"

namespace Web {

struct Server::Impl
{
    int64_t serverId {};

    ServerConfiguration configuration;
    std::set<DetectorHandler> detectors;
    bool cache_isServerAvailable {false};

    bool checkPort(const QString &host, quint16 port) const {
        QTcpSocket socket;
        socket.connectToHost(host, port);
        if (socket.waitForConnected(1000)) {
            socket.disconnectFromHost();
            return true;
        }
        return false;
    }

};

Server::Server(int64_t serverId, ServerRegistry *parent)
    : QObject{parent},
    d {new Impl()}
{
    d->serverId = serverId;
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
}

void Server::setPort(const uint16_t &port)
{
    d->configuration.setPort(port);
    emit configurationChanged();
}

void Server::setName(const QString &name)
{
    d->configuration.setName(name);
    emit configurationChanged();
}

void Server::requestPoweroff() const
{
    // TODO: Implement
}

void Server::requestReboot() const
{
    // TODO: Implement
}

void Server::requestStatus() const
{
    // TODO: Implement
}

const ServerConfiguration &Server::getConfiguration() const
{
    return d->configuration;
}

bool Server::addDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto pServer = new Detector(this);
    if (!pServer->setConfiguration(conf)) {
        pServer->deleteLater();
        return false;
    }

    // TODO: Register detector in remote

    DetectorHandler det(pServer);
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
    return d->detectors;
}

bool Server::operator<(const Server &s) const
{
    return d->configuration < s.d->configuration;
}

void Server::setConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;
}

} // namespace Web
