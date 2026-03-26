#include "servermanager.hpp"

#include <QNetworkReply>

#include <ROD/Protocol.h>

#include <Components/Logger/Logger.h>

ServerManager::ServerManager(QObject *parent)
    : HTTPClientBase{parent}
{

}

void ServerManager::requestStatus()
{
    auto statusRequest = createRequest(Protocol::API::QT::SERVER_STATUS.c_str());
    auto& requester = getRequester();
    auto response = requester.get(statusRequest);
    connect(response, &QNetworkReply::finished,
            this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        DataObjects::DeviceStatus status;

        if (!status.readJson(response->readAll().toStdString())) {
            COMPLOG_WARNING("Status parse error:", status.getLastErrorString());
        }

        emit responseStatus(isOk, status);
    });
}

void ServerManager::requestReboot()
{
    auto rebootRequest = createRequest(QString::fromStdString(Protocol::API::QT::SERVER_POWER).arg(Protocol::API::PARAMATER_VALUES::POWER_REBOOT));
    auto& requester = getRequester();
    auto response = requester.put(rebootRequest, QByteArray());
    connect(response, &QNetworkReply::finished,
            this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        emit responseReboot(isOk);
    });
}

void ServerManager::requestShutdown()
{
    auto shutdownRequest = createRequest(QString::fromStdString(Protocol::API::QT::SERVER_POWER).arg(Protocol::API::PARAMATER_VALUES::POWER_OFF));
    auto& requester = getRequester();
    auto response = requester.put(shutdownRequest, QByteArray());
    connect(response, &QNetworkReply::finished,
            this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        emit responseShutdown(isOk);
    });
}
