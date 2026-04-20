#include "serverregistry.hpp"

#include "server.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Database/SQlite.h>

namespace Web {

struct ServerRegistry::Impl
{
    Database::SQLiteDatabase db;
    std::unique_ptr<Database::SQLiteTable> serversDb;

    std::set<ServerHandler> servers;

    QString lastErrorText;
};

ServerRegistry::ServerRegistry(QObject *parent)
    : QObject{parent},
    d {new Impl()}
{

}

ServerRegistry::~ServerRegistry()
{

}

void ServerRegistry::init()
{
    COMPLOG_INFO("ServerRegistry init started...");

    auto dataDir = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::Data);
    d->db.setDatabase(dataDir / "local.db");
    d->serversDb = std::make_unique<Database::SQLiteTable>(d->db);

    // TODO: Init server list

    COMPLOG_OK("ServerRegistry inited");
}

bool ServerRegistry::addServer(const ServerConfiguration &conf)
{
    auto pServer = new Server(this);
    if (!pServer->setConfiguration(conf)) {
        pServer->deleteLater();
        d->lastErrorText = "Invalid server configuration";
        return false;
    }

    // TODO: Register server in local DB

    ServerHandler serv(pServer);
    d->servers.insert(serv);
    emit serverAdded(serv);
    return true;
}

void ServerRegistry::removeServer(const ServerConfiguration &conf)
{
    auto targetIt = std::find_if(d->servers.begin(), d->servers.end(), [&conf](const auto& serverHdl){
        return (serverHdl->getConfiguration() == conf);
    });
    if (targetIt == d->servers.end()) {
        return;
    }
    auto serverHdl = *targetIt;
    emit serverAboutToRemove(serverHdl);
    d->servers.erase(targetIt);

    // Delete safely
    auto pServ = serverHdl.get();
    serverHdl.invalidate();
    pServ->deleteLater();
}

std::set<ServerHandler> ServerRegistry::getServers() const
{
    return d->servers;
}

QString ServerRegistry::getLastErrorText() const
{
    return d->lastErrorText;
}

} // namespace Web
