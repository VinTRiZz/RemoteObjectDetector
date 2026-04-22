#include "detectorinfomanager.hpp"

#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>

#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>

#include "httpcommon.hpp"

namespace Web::Implementation
{

DetectorInfoManager::DetectorInfoManager(QObject *parent)
    : HTTPClientBase{parent}
{

}

QString DetectorInfoManager::getLastErrorText() const
{
    return m_lastErrorText;
}

DataObjects::id_t DetectorInfoManager::addDetectorInfo(const DataObjects::DetectorConfiguration &detectorConfig)
{
    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg("0"));
    auto& requester = getRequester();
    infoRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    auto response = requester.post(infoRequest, QByteArray::fromStdString(detectorConfig.toJson()));

    bool isOk {false};
    DataObjects::id_t id {DataObjects::NULL_ID};
    QString responseString;
    connect(response, &QNetworkReply::finished,
            this, [this, response, &isOk, &responseString](){
                isOk = response->error() == QNetworkReply::NoError;
                responseString = response->readAll();
            });

    QEventLoop loop;
    QObject::connect(response, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);
    loop.exec();

    if (isOk) {
        id = responseString.toLongLong();
        COMPLOG_OK("[DetectorInfoManager] Added detector with id:", id);
    } else {
        COMPLOG_ERROR("[DetectorInfoManager] Add detector with id", detectorConfig.system.id, "failed:", responseString.toStdString());
        m_lastErrorText = responseString;
    }
    return (isOk ? DataObjects::id_t(id) : std::nullopt);
}

bool DetectorInfoManager::removeDetectorInfo(DataObjects::id_t detectorId)
{
    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(detectorId.has_value() ? QString::number(detectorId.value()) : "0"));
    auto& requester = getRequester();
    auto response = requester.deleteResource(infoRequest);

    bool isOk {false};
    std::string responseString;
    connect(response, &QNetworkReply::finished,
            this, [this, response, &isOk, &responseString](){
                isOk = response->error() == QNetworkReply::NoError;
                responseString = response->readAll().toStdString();
            });

    QEventLoop loop;
    QObject::connect(response, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);
    loop.exec();

    if (isOk) {
        COMPLOG_OK("[DetectorInfoManager] Removed detector with id:", detectorId);
    } else {
        COMPLOG_ERROR("[DetectorInfoManager] Remove detector with id", detectorId, "failed:", responseString);
        m_lastErrorText = responseString.c_str();
    }
    return isOk;
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

void DetectorInfoManager::requestGetDetectorInfo(DataObjects::id_t detectorId)
{
    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(detectorId.has_value() ? QString::number(detectorId.value()) : "0"));
    auto& requester = getRequester();
    auto response = requester.get(infoRequest);
    connect(response, &QNetworkReply::finished,
        this, [this, response](){
        auto isOk = response->error() == QNetworkReply::NoError;
        DataObjects::DetectorConfiguration detectorConfig;

        if (!detectorConfig.readJson(response->readAll().toStdString())) {
            COMPLOG_WARNING("[DetectorInfoManager] Detector info parse error:", detectorConfig.getLastErrorString());
        }

        emit responseGetDetectorInfo(isOk, detectorConfig);
    });
}

void DetectorInfoManager::requestUpdateDetectorInfo(const DataObjects::DetectorConfiguration &detectorConfig)
{
    auto detectorId = detectorConfig.system.id;

    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(detectorId.has_value() ? QString::number(detectorId.value()) : "0"));
    infoRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    auto& requester = getRequester();
    auto response = requester.put(infoRequest, QByteArray::fromStdString(detectorConfig.toJson()));

    connect(response, &QNetworkReply::finished,
            this, [this, response, detectorId](){
                auto isOk = response->error() == QNetworkReply::NoError;
                if (!isOk) {
                    COMPLOG_ERROR("[DetectorInfoManager] Update detector with id", detectorId, "failed:", response->readAll().toStdString());
                }
                emit responseUpdateDetectorInfo(isOk, detectorId);
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
                    m_lastErrorText = "Detector list process failed";
                    emit responseDetectorInfoList(false, {});
                    return;
                }

                std::shared_ptr<std::vector<DataObjects::DetectorConfiguration> > configs =
                    std::make_shared<std::vector<DataObjects::DetectorConfiguration> >();
                configs->reserve(idVect.size());

                std::size_t totalAmount = idVect.size();
                for (auto& id : idVect) {
                    auto infoRequest = createRequest(QString::fromStdString(Protocol::API::QT::DETECTOR_INFO).arg(id.has_value() ? QString::number(id.value()) : "0"));
                    auto& requester = getRequester();
                    auto response = requester.get(infoRequest);
                    connect(response, &QNetworkReply::finished,
                            this, [this, response, totalAmount, configs]() mutable {
                                auto isOk = response->error() == QNetworkReply::NoError;
                                DataObjects::DetectorConfiguration detectorConfig;

                                if (!detectorConfig.readJson(response->readAll().toStdString())) {
                                    COMPLOG_WARNING("[DetectorInfoManager] Detector info parse error:", detectorConfig.getLastErrorString());
                                    m_lastErrorText = "Response process failed";
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