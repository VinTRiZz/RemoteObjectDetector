#pragma once

#include "httpclientbase.hpp"
#include <ROD/Protocol.h>
#include <ROD/DeviceStatus.h>

namespace Web::Implementation
{

/**
 * @brief The ServerManager class Server manager
 */
class ServerManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit ServerManager(QObject *parent = nullptr);

public slots:
    void requestStatus();
    void requestReboot();
    void requestPoweroff();

signals:
    void responseStatus(bool isOk, const DataObjects::DeviceStatus& serverStatus);
    void responseReboot(bool isOk);
    void responseShutdown(bool isOk);
};

}