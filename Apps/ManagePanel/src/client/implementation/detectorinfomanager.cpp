#include "detectorinfomanager.hpp"

#include <QNetworkReply>

#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>

#include "httpcommon.hpp"

DetectorInfoManager::DetectorInfoManager(QObject *parent)
    : HTTPClientBase{parent}
{

}

void DetectorInfoManager::requestDetectorList()
{
    auto infoRequest = createRequest(Protocol::API::QT::DETECTOR_GET_ID_LIST.c_str());
    auto& requester = getRequester();
    auto response = requester.get(infoRequest);
    connect(response, &QNetworkReply::finished,
        this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        DataObjects::DetectorConfiguration detectorConfig;

        std::vector<DataObjects::id_t> idVect;

        auto valueArray = HTTPCommon::parseBodyArray(response->readAll());
        if (std::holds_alternative<QJsonArray>(valueArray)) {
            auto& valueArr = std::get<QJsonArray>(valueArray);
            for (auto valRef : valueArr) {
                idVect.push_back(valRef.toInt());
            }
        } else {
            COMPLOG_ERROR("[DetectorInfoManager] Failed to parse id list:", std::get<QString>(valueArray).toStdString());
        }

        emit responseDetectorList(isOk, idVect);
    });
}

void DetectorInfoManager::requestDetectorInfo(DataObjects::id_t detectorId)
{
    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(QString::number(detectorId)));
    auto& requester = getRequester();
    auto response = requester.get(infoRequest);
    connect(response, &QNetworkReply::finished,
        this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        DataObjects::DetectorConfiguration detectorConfig;

        if (!detectorConfig.readJson(response->readAll().toStdString())) {
            COMPLOG_WARNING("[DetectorInfoManager] Detector info parse error:", detectorConfig.getLastErrorString());
        }

        emit responseDetectorInfo(isOk, detectorConfig);
    });
}
