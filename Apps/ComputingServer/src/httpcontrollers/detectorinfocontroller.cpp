#include "detectorinfocontroller.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

void DetectorInfoController::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_deviceInfoManager.setRecordManager(pManager);
    m_deviceInfoManager.updateDetectorsInfo();
}

void DetectorInfoController::processGetList(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    nlohmann::json res;
    for (auto id : m_deviceInfoManager.getDetectorList()) {
        res.push_back(id);
    }
    sendJsonMessage(drogon::k200OK, res.dump(), std::move(callback));
}

void DetectorInfoController::processAddInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t::type detectorId)
{
    auto detectorInfoJson = req->bodyData();
    DataObjects::DetectorConfiguration detConfig;
    if (!detConfig.readJson(detectorInfoJson)) {
        sendTextMessage(drogon::k400BadRequest, "Invalid detector configuration data", std::move(callback));
        return;
    }
    auto savedId = m_deviceInfoManager.addDetector(detConfig);
    if (savedId.has_value()) {
        sendTextMessage(drogon::k200OK, std::to_string(savedId.value()), std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, "Failed to save detector data", std::move(callback));
}

void DetectorInfoController::processGetInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t::type detectorId)
{
    auto detectorInfo = m_deviceInfoManager.getDetectorInfo(detectorId);
    if (!detectorInfo.has_value()) {
        sendTextMessage(drogon::k404NotFound, "Invalid detector ID", std::move(callback));
        return;
    }
    sendJsonMessage(drogon::k200OK, detectorInfo->toJson(), std::move(callback));
}

void DetectorInfoController::processUpdateInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t::type detectorId)
{
    auto detectorInfoJson = req->bodyData();
    DataObjects::DetectorConfiguration detConfig;
    if (!detConfig.readJson(detectorInfoJson)) {
        sendTextMessage(drogon::k400BadRequest, "Invalid detector configuration data", std::move(callback));
        return;
    }
    if (m_deviceInfoManager.updateDetectorData(detConfig)) {
        sendTextMessage(drogon::k200OK, "Detector data changed", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, "Failed to save changed data", std::move(callback));
}

void DetectorInfoController::processRemoveInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t::type detectorId)
{
    if (m_deviceInfoManager.removeDetectorData(detectorId)) {
        sendTextMessage(drogon::k200OK, "Detector removed", std::move(callback));
        return;
    }
    sendTextMessage(drogon::k500InternalServerError, "Failed to remove detector data", std::move(callback));
}

void DetectorInfoController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t::type detectorId)
{
    sendTextMessage(drogon::k501NotImplemented, "Detector status not implemented", std::move(callback));
}
