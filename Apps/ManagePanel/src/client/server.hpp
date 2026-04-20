#pragma once

#include <QObject>
#include <memory>
#include <set>

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
    explicit Server(ServerRegistry *parent = nullptr);
    ~Server();

    void requestPoweroff() const;
    void requestReboot() const;
    void requestStatus() const;

    const ServerConfiguration& getConfiguration() const;

    bool addDetector(const DataObjects::DetectorConfiguration& conf);
    void removeDetector(const DataObjects::DetectorConfiguration& conf);
    std::set<DetectorHandler> getDetectors() const;

    bool operator<(const Server& s) const;
signals:
    void detectorAdded(const DetectorHandler& hdl);
    void detectorAboutToRemove(const DetectorHandler& hdl);

    void gotStatus(const DataObjects::DeviceStatus& devStatus);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    friend class ServerRegistry;
    bool setConfiguration(const ServerConfiguration& conf);
};

} // namespace Web
