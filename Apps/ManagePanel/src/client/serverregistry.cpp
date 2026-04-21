#include "serverregistry.hpp"

#include "server.hpp"
#include "detectorserver.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Database/SQlite.h>

#include <QApplication>

namespace Web {

struct ServerRegistry::Impl
{
    Database::SQLiteDatabase db;
    std::unique_ptr<Database::SQLiteTable> serversTable;

    std::set<ServerHandler> servers;

    QString lastErrorText;

    bool createTable() {
        serversTable = std::make_unique<Database::SQLiteTable>(db);
        serversTable->setTable("servers");
        if (serversTable->isTableExist()) {
            return true;
        }

        std::list<Database::SQLiteTable::ColumnInfo> cols;

        Database::SQLiteTable::ColumnInfo col;
        col.name = "id";
        col.defaultValue = {};
        col.canBeNull = false;
        col.isPrimaryKey = true;
        col.type = Database::CT_INTEGER;
        cols.push_back(col);

        col = {};
        col.name = "host";
        col.defaultValue = {};
        col.canBeNull = false;
        col.type = Database::CT_TEXT;
        cols.push_back(col);

        col = {};
        col.name = "name";
        col.defaultValue = {};
        col.canBeNull = false;
        col.type = Database::CT_TEXT;
        cols.push_back(col);

        col = {};
        col.name = "port";
        col.defaultValue = {};
        col.canBeNull = false;
        col.type = Database::CT_INTEGER;
        cols.push_back(col);

        return serversTable->create(cols);
    }

    void registerServer(const ServerConfiguration& conf) {
        std::map<std::string, Database::DBCell> rowData;
        rowData["host"]     = conf.getHost().toStdString();
        rowData["port"]     = static_cast<Database::DBCellInteger>(conf.getPort());
        rowData["name"]     = conf.getName().toStdString();
        if (!serversTable->addRow(std::move(rowData))) {
            COMPLOG_WARNING("Failed to register server:", conf.getName().toStdString());
        }
    }

    bool updateServerConfiguration(int64_t id, const ServerConfiguration& conf) {
        std::map<std::string, Database::DBCell> rowData;
        rowData["host"]     = conf.getHost().toStdString();
        rowData["port"]     = static_cast<Database::DBCellInteger>(conf.getPort());
        rowData["name"]     = conf.getName().toStdString();
        return serversTable->updateRow(std::move(rowData), std::string("id = ") + std::to_string(id));
    }
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
    if (!d->createTable()) {
        COMPLOG_ERROR("ServerRegistry: failed to create servers table");
        qApp->quit(); // Bad, but required (app must not work with this trouble)
        return;
    }

    auto records = d->serversTable->getRows({"id", "host", "port", "name"}, {}, "id ASC");
    for (auto& serverRec : records) {
        ServerConfiguration conf;
        conf.setHost(std::get<Database::DBCellString>(serverRec[1]).value().c_str());
        conf.setPort(std::get<Database::DBCellInteger>(serverRec[2]).value());
        conf.setName(std::get<Database::DBCellString>(serverRec[3]).value().c_str());
        addServerNoRegister(std::get<Database::DBCellInteger>(serverRec[0]).value(), conf);
    }

    COMPLOG_OK("ServerRegistry inited");
}

bool ServerRegistry::addServer(const ServerConfiguration &conf)
{
    d->registerServer(conf);
    auto nextId = std::get<Database::DBCellInteger>(d->serversTable->getRows({"MAX(id) + 1"}).front().front()).value();
    addServerNoRegister(nextId, conf);
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

void ServerRegistry::addServerNoRegister(int64_t serverId, const ServerConfiguration &conf)
{
    // TODO: Assume using configuration, what type of server it is
    auto pServer = new DetectorServer(serverId, this);
    connect(pServer, &Server::configurationChanged,
            this, [this, pServer](){
        if (!d->updateServerConfiguration(pServer->getId(), pServer->getConfiguration())) {
            COMPLOG_WARNING("Failed to update configuration of server:", pServer->getConfiguration().getName().toStdString());
        }
    });

    pServer->replaceConfiguration(conf);
    ServerHandler serv(pServer);
    d->servers.insert(serv);
    emit serverAdded(serv);
    pServer->ping(); // Update state
}

} // namespace Web
