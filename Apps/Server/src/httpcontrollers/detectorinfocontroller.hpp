#pragma once

#include <drogon/drogon.h>

#include <ROD/Protocol.h>

#include "controllerbase.hpp"

#include "detector/detectorinfomanager.hpp"

class DetectorInfoController : public drogon::HttpController<DetectorInfoController, false>,
                               public ControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DetectorInfoController::processGetStatus,   Protocol::API::DROGON::DETECTOR_STATUS,   drogon::Get);
    METHOD_LIST_END

    void setRecordManager(const Database::RecordManagerPtr& pManager);

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetStatus(const drogon::HttpRequestPtr &req,
                          ResponseCallback_t &&callback);

private:
    DetectorInfoManager m_deviceInfoManager;
};
