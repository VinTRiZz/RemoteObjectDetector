#pragma once

#include <drogon/drogon.h>
#include "detector/detectorcommandprocessor.hpp"

#include <ROD/Protocol.h>

/**
 * @brief The DeviceSoftVersionController class Контроллер, отвечающий за версию ПО, установленную на устройствах
 */
class DeviceSoftVersionController : public drogon::HttpController<DeviceSoftVersionController, false>
{
public:
    DeviceSoftVersionController(const std::shared_ptr<DetectorCommandProcessor>& detectorCommandProcessor);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DeviceSoftVersionController::processGetSoftVersion,  Protocol::API::DROGON::DETECTOR_APP_VERSION_GET, drogon::Get);
        ADD_METHOD_TO(DeviceSoftVersionController::processAddVersion,      Protocol::API::DROGON::DETECTOR_APP_VERSION_ADD,   drogon::Post);
        ADD_METHOD_TO(DeviceSoftVersionController::processSetSoftVersion,  Protocol::API::DROGON::DETECTOR_APP_VERSION_SET, drogon::Put);
        ADD_METHOD_TO(DeviceSoftVersionController::processRemoveVersion,   Protocol::API::DROGON::DETECTOR_APP_VERSION_REM, drogon::Delete);
    METHOD_LIST_END

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetSoftVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const std::string& deviceUuid);

    void processAddVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const std::string& versionUuid,
                        const std::string& versionName);

    void processSetSoftVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const std::string& deviceUuid,
                        const std::string& versionUuid);

    void processRemoveVersion(const drogon::HttpRequestPtr &req,
                        ResponseCallback_t &&callback,
                        const std::string& versionUuid);

private:
    std::shared_ptr<DetectorCommandProcessor>   m_detectorCommandProcessor;
};

