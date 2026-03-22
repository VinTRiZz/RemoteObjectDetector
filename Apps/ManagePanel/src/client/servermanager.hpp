#pragma once

#include "httpclientbase.hpp"
#include "apidatastructures.hpp"

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
    void requestShutdown();

signals:
    void responseStatus(bool isOk, const API::Structures::ServerStatus& serverStatus);
    void responseReboot(bool isOk);
    void responseShutdown(bool isOk);
};
