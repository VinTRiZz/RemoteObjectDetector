#include "server.hpp"

#include "common/serverconfiguration.hpp"

#include "serverregistry.hpp"
#include "detector.hpp"

namespace Web {

struct Server::Impl
{
    ServerConfiguration configuration;
    std::set<DetectorHandler> detectors;
};

Server::Server(ServerRegistry *parent)
    : QObject{parent},
    d {new Impl()}
{

}

Server::~Server()
{
    // TODO: Disconnect if need
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
    detHdl->deleteLater();
}

std::set<DetectorHandler> Server::getDetectors() const
{
    return d->detectors;
}

bool Server::operator<(const Server &s) const
{
    return d->configuration < s.d->configuration;
}

bool Server::setConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;

    // TODO: Check config if server is available

    return true;
}

} // namespace Web
