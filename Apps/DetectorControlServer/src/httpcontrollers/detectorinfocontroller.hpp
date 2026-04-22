#pragma once

#include <drogon/drogon.h>

#include <ROD/Exchange/Events.h>

#include <ROD/Servers/ControllerBase.h>

#include "detector/detectorinfomanager.hpp"

class DetectorInfoController : public drogon::HttpController<DetectorInfoController, false>,
                               public ControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DetectorInfoController::processGetList,       Protocol::API::DROGON::DETECTOR_GET_ID_LIST,    drogon::Get);
        ADD_METHOD_TO(DetectorInfoController::processAddInfo,       Protocol::API::DROGON::DETECTOR_INFO,           drogon::Post);
        ADD_METHOD_TO(DetectorInfoController::processGetInfo,       Protocol::API::DROGON::DETECTOR_INFO,           drogon::Get);
        ADD_METHOD_TO(DetectorInfoController::processUpdateInfo,    Protocol::API::DROGON::DETECTOR_INFO,           drogon::Put);
        ADD_METHOD_TO(DetectorInfoController::processRemoveInfo,    Protocol::API::DROGON::DETECTOR_INFO,           drogon::Delete);
        ADD_METHOD_TO(DetectorInfoController::processGetStatus,     Protocol::API::DROGON::DETECTOR_STATUS,         drogon::Get);
    METHOD_LIST_END

    void setRecordManager(const Database::RecordManagerPtr& pManager);

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetList(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback);

    void processAddInfo(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t::type detectorId);
    void processGetInfo(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t::type detectorId);
    void processUpdateInfo(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t::type detectorId);
    void processRemoveInfo(const drogon::HttpRequestPtr &req,
                           ResponseCallback_t &&callback,
                           const DataObjects::id_t::type detectorId);

    void processGetStatus(const drogon::HttpRequestPtr &req,
                          ResponseCallback_t &&callback,
                          const DataObjects::id_t::type detectorId);

private:
    DetectorInfoManager m_deviceInfoManager;
};
