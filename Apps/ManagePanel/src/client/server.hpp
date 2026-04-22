#pragma once

#include <QObject>
#include <memory>
#include <optional>

#include <ROD/DataObjects/DetectorConfiguration.h>
#include <ROD/DataObjects/DeviceStatus.h>

#include "handlers.hpp"
#include "common/commitableobject.hpp"

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

    bool isServerListening(const QString &host, quint16 port) const;

    int64_t getId() const;

    void ping();
    bool isServerAvailable() const; // Returns last ping result

    void requestPoweroff() const;
    void requestReboot() const;
    void requestStatus() const;

    virtual void setConfiguration(const ServerConfiguration& conf);
    const ServerConfiguration& getConfiguration() const;

    bool operator<(const Server& s) const;
signals:
    void visibleDataChanged();

    void gotStatus(const DataObjects::DeviceStatus& devStatus);

    void configurationChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    friend class ServerRegistry;
    void replaceConfiguration(const ServerConfiguration& conf);
    void commitConfigurationUpdate();

protected:
    virtual void updateServerAddress(); // Must be called last in overriden functions
};

} // namespace Web
