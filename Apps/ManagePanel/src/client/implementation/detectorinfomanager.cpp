#include "detectorinfomanager.hpp"

#include <QNetworkReply>

#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>

#include "httpcommon.hpp"

namespace Web::Implementation
{

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

void DetectorInfoManager::requestDetectorInfoList()
{
    // TODO: Make readable
    auto infoRequest = createRequest(Protocol::API::QT::DETECTOR_GET_ID_LIST.c_str());
    auto& requester = getRequester();
    auto response = requester.get(infoRequest);
    connect(response, &QNetworkReply::finished,
            this, [this, response](){
                auto isOk = response->error() == QNetworkReply::NoError;
                std::vector<DataObjects::id_t> idVect;

                auto valueArray = HTTPCommon::parseBodyArray(response->readAll());
                if (std::holds_alternative<QJsonArray>(valueArray)) {
                    auto& valueArr = std::get<QJsonArray>(valueArray);
                    for (auto valRef : valueArr) {
                        idVect.push_back(valRef.toInt());
                    }
                } else {
                    COMPLOG_ERROR("[DetectorInfoManager] Failed to parse id list:", std::get<QString>(valueArray).toStdString());
                    emit responseDetectorInfoList(false, {});
                    return;
                }

                std::shared_ptr<std::vector<DataObjects::DetectorConfiguration> > configs =
                    std::make_shared<std::vector<DataObjects::DetectorConfiguration> >();
                configs->reserve(idVect.size());

                std::size_t totalAmount = idVect.size();
                for (auto& id : idVect) {
                    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(QString::number(id)));
                    auto& requester = getRequester();
                    auto response = requester.get(infoRequest);
                    connect(response, &QNetworkReply::finished,
                            this, [this, response, totalAmount, configs]() mutable {
                                auto isOk = response->error() == QNetworkReply::NoError;
                                DataObjects::DetectorConfiguration detectorConfig;

                                if (!detectorConfig.readJson(response->readAll().toStdString())) {
                                    COMPLOG_WARNING("[DetectorInfoManager] Detector info parse error:", detectorConfig.getLastErrorString());
                                    emit responseDetectorInfoList(false, {});
                                    return;
                                }
                                configs->emplace_back(std::move(detectorConfig));

                                if (!isOk || totalAmount == configs->size()) {
                                    emit responseDetectorInfoList(isOk, *configs);
                                }
                            });
                }
            });
}

}