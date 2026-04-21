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
        ADD_METHOD_TO(DetectorInfoController::processGetList,       Protocol::API::DROGON::DETECTOR_GET_ID_LIST,    drogon::Get);
        ADD_METHOD_TO(DetectorInfoController::processGetInfo,       Protocol::API::DROGON::DETECTOR_INFO,           drogon::Get);
        ADD_METHOD_TO(DetectorInfoController::processGetStatus,     Protocol::API::DROGON::DETECTOR_STATUS,         drogon::Get);
    METHOD_LIST_END

    void setRecordManager(const Database::RecordManagerPtr& pManager);

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetList(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback);

    void processGetInfo(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t detectorId);

    void processGetStatus(const drogon::HttpRequestPtr &req,
                          ResponseCallback_t &&callback,
                          const DataObjects::id_t detectorId);

private:
    DetectorInfoManager m_deviceInfoManager;
};
