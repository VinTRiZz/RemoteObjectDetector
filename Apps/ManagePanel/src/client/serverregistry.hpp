#pragma once

#include <QObject>

#include "handlers.hpp"
#include "common/serverconfiguration.hpp"

#include <memory>
#include <set>

namespace Web {

class Server;

/**
 * @brief The ServerRepository class Provides data about servers
 */
class ServerRegistry : public QObject
{
    Q_OBJECT
public:
    explicit ServerRegistry(QObject *parent = nullptr);
    ~ServerRegistry();

    void init();

    bool addServer(const ServerConfiguration& conf);
    void removeServer(const ServerConfiguration& conf);

    std::set<ServerHandler> getServers() const;

    QString getLastErrorText() const;

signals:
    void initSucceed();
    void serverAdded(const ServerHandler& hdl);
    void serverAboutToRemove(const ServerHandler& hdl);

private:
    struct Impl;
    std::shared_ptr<Impl> d;
};

} // namespace Web
