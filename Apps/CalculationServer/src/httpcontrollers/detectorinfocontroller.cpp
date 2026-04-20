#include "detectorinfocontroller.hpp"

#include <nlohmann/json.hpp>

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

void DetectorInfoController::processGetInfo(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t detectorId)
{
    auto detectorInfo = m_deviceInfoManager.getDetectorInfo(detectorId);
    if (!detectorInfo.has_value()) {
        sendTextMessage(drogon::k404NotFound, "Invalid detector ID", std::move(callback));
        return;
    }
    sendJsonMessage(drogon::k200OK, detectorInfo->toJson(), std::move(callback));
}

void DetectorInfoController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const DataObjects::id_t detectorId)
{
    sendTextMessage(drogon::k501NotImplemented, "Detector status not implemented", std::move(callback));
}
