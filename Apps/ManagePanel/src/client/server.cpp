#include "server.hpp"

#include <QTcpSocket>

#include <Components/Logger/Logger.h>

#include "implementation/servermanager.hpp"
#include "common/serverconfiguration.hpp"
#include "serverregistry.hpp"

namespace Web {

using namespace Implementation;

struct Server::Impl
{
    // Common server properties
    int64_t             serverId {};
    ServerManager       serverInterface;
    CommitableObject<ServerConfiguration> configuration;
    bool                cache_isServerAvailable {false};
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

bool Server::isServerListening(const QString &host, quint16 port) const {
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if (socket.waitForConnected(1000)) {
        socket.disconnectFromHost();
        return true;
    }
    return false;
}

int64_t Server::getId() const
{
    return d->serverId;
}

void Server::ping()
{
    QMetaObject::invokeMethod(this, [this](){
        if (isServerListening(d->configuration->getHost(), d->configuration->getPort())) {
            d->cache_isServerAvailable = true;
        } else {
            d->cache_isServerAvailable = false;
        }
        emit visibleDataChanged();
    });
}

bool Server::isServerAvailable() const
{
    return d->cache_isServerAvailable;
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

void Server::setConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;
    emit configurationChanged();
}

const ServerConfiguration &Server::getConfiguration() const
{
    return d->configuration.source();
}

bool Server::operator<(const Server &s) const
{
    return d->configuration < s.d->configuration;
}

void Server::replaceConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;
    d->configuration.commit();
    updateServerAddress();
    emit visibleDataChanged();
}

void Server::commitConfigurationUpdate()
{
    d->configuration.commit();
    updateServerAddress();
    emit visibleDataChanged();
}

void Server::updateServerAddress()
{
    auto serverAddr = d->configuration->getHost() + ":" + QString::number(d->configuration->getPort());
    d->serverInterface.setServer(serverAddr);
    ping();
}

} // namespace Web
