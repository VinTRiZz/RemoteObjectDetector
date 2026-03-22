#include "servermanager.hpp"

#include <QNetworkReply>

#include <ROD/Protocol.h>

#include <Components/Logger/Logger.h>

#include "httpcommon.hpp"

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
        API::Structures::ServerStatus status;

        auto responseBody = response->readAll();
        auto jsonData = HTTPCommon::parseBody(responseBody);
        if (std::holds_alternative<QString>(jsonData)) {
            COMPLOG_WARNING("Status parse error:", std::get<QString>(jsonData).toStdString());
        } else {
            auto& statusJson = std::get<QJsonObject>(jsonData);

            status.common.uptime = statusJson["common"].toObject()["uptime"].toInt();

            status.cpu.loadPercent = statusJson["cpu"].toObject()["load"].toDouble();
            status.cpu.temperature = statusJson["cpu"].toObject()["temp"].toDouble();

            // Funny, that "uintmax_t" in nlohmann::json is double in Qt
            status.storage.spaceTotal       = statusJson["storage"].toObject()["space_total"].toDouble();
            status.storage.spaceAvailable   = statusJson["storage"].toObject()["space_available"].toDouble();
            status.storage.spaceFree        = statusJson["storage"].toObject()["space_free"].toDouble();

            COMPLOG_OK("Parsed status. Body:", responseBody.toStdString());
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
