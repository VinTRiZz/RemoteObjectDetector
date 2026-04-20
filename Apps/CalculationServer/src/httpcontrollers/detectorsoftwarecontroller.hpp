#pragma once

#include <drogon/drogon.h>

#include "controllerbase.hpp"

#include "eventprocessors/detectorcommandprocessor.hpp"
#include "common/servercommon.hpp"

#include <ROD/Protocol.h>

/**
 * @brief The DetectorSoftwareController class Контроллер, отвечающий за версию ПО, установленную на устройствах
 */
class DetectorSoftwareController : public drogon::HttpController<DetectorSoftwareController, false>,
                                    public ControllerBase
{
public:
    DetectorSoftwareController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DetectorSoftwareController::processGetExistingVersions,  Protocol::API::DROGON::DETECTOR_APP_VERSION_GET_ALL,drogon::Get);
        ADD_METHOD_TO(DetectorSoftwareController::processGetSoftVersion,       Protocol::API::DROGON::DETECTOR_APP_VERSION_GET,    drogon::Get);
        ADD_METHOD_TO(DetectorSoftwareController::processAddVersion,           Protocol::API::DROGON::DETECTOR_APP_VERSION_ADD,    drogon::Post);
        ADD_METHOD_TO(DetectorSoftwareController::processSetSoftVersion,       Protocol::API::DROGON::DETECTOR_APP_VERSION_SET,    drogon::Put);
        ADD_METHOD_TO(DetectorSoftwareController::processRemoveVersion,        Protocol::API::DROGON::DETECTOR_APP_VERSION_REM,    drogon::Delete);
    METHOD_LIST_END

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetExistingVersions(const drogon::HttpRequestPtr &req,
                               ResponseCallback_t &&callback);

    void processGetSoftVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t& deviceId);

    void processAddVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback);

    void processSetSoftVersion(const drogon::HttpRequestPtr &req,
                               ResponseCallback_t &&callback,
                               const DataObjects::id_t& deviceId,
                               const DataObjects::id_t& versionId);

    void processRemoveVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const DataObjects::id_t& versionId);

    std::shared_ptr<DetectorCommandProcessor> getDetectorCommandProcessor() const;

private:
    DeviceSoftwareManager                       m_deviceSoftwareManager;
    std::shared_ptr<DetectorCommandProcessor>   m_detectorCommandProcessor;
};

