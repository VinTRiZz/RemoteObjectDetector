#include "detectorinfocontroller.hpp"

void DetectorInfoController::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_deviceInfoManager.setRecordManager(pManager);
    m_deviceInfoManager.init();
}

void DetectorInfoController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    sendTextMessage(drogon::k501NotImplemented, "Detector status not implemented", std::move(callback));
}
