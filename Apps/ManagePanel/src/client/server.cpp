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
    int64_t serverId {};
    ServerManager serverInterface;
    ServerConfiguration configuration;
    bool                cache_isServerAvailable {false};

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
    updateServerAddress();
}

void Server::setPort(const uint16_t &port)
{
    d->configuration.setPort(port);
    emit configurationChanged();
    updateServerAddress();
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

void Server::setConfiguration(const ServerConfiguration &conf)
{
    d->configuration = conf;
    updateServerAddress();
    emit configurationChanged();
}

const ServerConfiguration &Server::getConfiguration() const
{
    return d->configuration;
}

bool Server::operator<(const Server &s) const
{
    return d->configuration < s.d->configuration;
}

void Server::updateServerAddress()
{
    auto serverAddr = d->configuration.getHost() + ":" + QString::number(d->configuration.getPort());
    d->serverInterface.setServer(serverAddr);
    ping();
}

} // namespace Web
