#include "serverregistry.hpp"

#include "server.hpp"
#include "detectorserver.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/DirectoryManager.h>

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QUuid>
#include <QVariant>

namespace Web {

struct ServerRegistry::Impl
{
    QSqlDatabase db;
    QString      connectionName;

    std::set<ServerHandler> servers;

    QString lastErrorText;

    Impl()
        : connectionName(QStringLiteral("ServerRegistry_%1")
                             .arg(QUuid::createUuid().toString(QUuid::WithoutBraces)))
    {
    }

    ~Impl()
    {
        if (db.isValid()) {
            db.close();
        }
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }

    bool openDatabase(const QString& databasePath) {
        if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
            lastErrorText = QStringLiteral("SQLite driver is not available");
            return false;
        }

        db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
        db.setDatabaseName(databasePath);
        if (!db.open()) {
            lastErrorText = db.lastError().text();
            return false;
        }
        return true;
    }

    bool createTable() {
        QSqlQuery query(db);
        if (!query.exec(QStringLiteral(
                "CREATE TABLE IF NOT EXISTS servers ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "host TEXT NOT NULL, "
                "name TEXT NOT NULL, "
                "port INTEGER NOT NULL)"))) {
            lastErrorText = query.lastError().text();
            return false;
        }
        return true;
    }

    bool registerServer(const ServerConfiguration& conf, int64_t& assignedId) {
        QSqlQuery query(db);
        query.prepare(QStringLiteral(
            "INSERT INTO servers (host, name, port) VALUES (:host, :name, :port)"));
        query.bindValue(QStringLiteral(":host"), conf.getHost());
        query.bindValue(QStringLiteral(":name"), conf.getName());
        query.bindValue(QStringLiteral(":port"), static_cast<int>(conf.getPort()));
        if (!query.exec()) {
            lastErrorText = query.lastError().text();
            return false;
        }
        assignedId = query.lastInsertId().toLongLong();
        return true;
    }

    bool updateServerConfiguration(int64_t id, const ServerConfiguration& conf) {
        QSqlQuery query(db);
        query.prepare(QStringLiteral(
            "UPDATE servers SET host = :host, name = :name, port = :port WHERE id = :id"));
        query.bindValue(QStringLiteral(":host"), conf.getHost());
        query.bindValue(QStringLiteral(":name"), conf.getName());
        query.bindValue(QStringLiteral(":port"), static_cast<int>(conf.getPort()));
        query.bindValue(QStringLiteral(":id"), static_cast<qlonglong>(id));
        if (!query.exec()) {
            lastErrorText = query.lastError().text();
            return false;
        }
        return true;
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

    auto dataDir = Common::DirectoryManager::getDirectoryStatic(Common::Data);
    const auto databasePath = QString::fromStdString((dataDir / "local.db").string());
    if (!d->openDatabase(databasePath)) {
        COMPLOG_ERROR("ServerRegistry: failed to open database", databasePath.toStdString(),
                      "reason:", d->lastErrorText.toStdString());
        qApp->quit(); // Bad, but required (app must not work with this trouble)
        return;
    }

    if (!d->createTable()) {
        COMPLOG_ERROR("ServerRegistry: failed to create servers table", d->lastErrorText.toStdString());
        qApp->quit(); // Bad, but required (app must not work with this trouble)
        return;
    }

    QSqlQuery query(d->db);
    if (!query.exec(QStringLiteral("SELECT id, host, port, name FROM servers ORDER BY id ASC"))) {
        d->lastErrorText = query.lastError().text();
        COMPLOG_WARNING("ServerRegistry: failed to load servers:", d->lastErrorText.toStdString());
    } else {
        while (query.next()) {
            ServerConfiguration conf;
            conf.setHost(query.value(1).toString());
            conf.setPort(static_cast<uint16_t>(query.value(2).toInt()));
            conf.setName(query.value(3).toString());
            addServerNoRegister(query.value(0).toLongLong(), conf);
        }
    }

    COMPLOG_OK("ServerRegistry inited");
}

bool ServerRegistry::addServer(const ServerConfiguration &conf)
{
    int64_t serverId {0};
    if (!d->registerServer(conf, serverId)) {
        COMPLOG_WARNING("Failed to register server:", conf.getName().toStdString(),
                        d->lastErrorText.toStdString());
        return false;
    }
    addServerNoRegister(serverId, conf);
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
            COMPLOG_WARNING("Failed to update configuration of server:",
                            pServer->getConfiguration().getName().toStdString(),
                            d->lastErrorText.toStdString());
        }
    });

    pServer->replaceConfiguration(conf);
    ServerHandler serv(pServer);
    d->servers.insert(serv);
    emit serverAdded(serv);
    pServer->ping(); // Update state
}

} // namespace Web
