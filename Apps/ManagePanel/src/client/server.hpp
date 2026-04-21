#pragma once

#include <QObject>
#include <memory>

#include <ROD/DetectorConfiguration.h>
#include <ROD/DeviceStatus.h>

#include "handlers.hpp"

class ServerConfiguration;

namespace Web {

class ServerRegistry;

/**
 * @brief The Server class Object to work with server and it's data
 */
class Server : public QObject
{
    Q_OBJECT
public:
    Server(int64_t serverId, ServerRegistry *parent = nullptr);
    ~Server();

    int64_t getId() const;

    void ping();
    bool isServerAvailable() const; // Returns last ping result

    void setHost(const QString& hostname);
    void setPort(const uint16_t& port);
    void setName(const QString& name);

    void requestPoweroff() const;
    void requestReboot() const;
    void requestStatus() const;

    virtual void setConfiguration(const ServerConfiguration& conf);
    const ServerConfiguration& getConfiguration() const;

    bool operator<(const Server& s) const;
signals:
    void gotStatus(const DataObjects::DeviceStatus& devStatus);

    void serverIsAvailable();
    void serverIsUnavailable();

    void configurationChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

protected:
    virtual void updateServerAddress(); // Must be called last in overriden functions
};

} // namespace Web
