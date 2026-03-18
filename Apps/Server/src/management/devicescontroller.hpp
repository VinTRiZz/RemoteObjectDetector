#pragma once

#include <drogon/drogon.h>

#include <ROD/Protocol.h>

#include "detector/detectorcommandprocessor.hpp"

/**
 * @brief The DevicesController class Контроллер, отвечающий за управление детекторами
 */
class DevicesController : public drogon::HttpController<DevicesController, false>
{
public:
    DevicesController(const std::shared_ptr<DetectorCommandProcessor>& detectorCommandProcessor);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DevicesController::processGetDeviceStatus,        Protocol::API::DROGON::DETECTOR_STATUS,             drogon::Get);
        ADD_METHOD_TO(DevicesController::processDevicePowerRequest,     Protocol::API::DROGON::DETECTOR_POWER,              drogon::Put);
        ADD_METHOD_TO(DevicesController::processToggleStreamingMode,    Protocol::API::DROGON::DETECTOR_TOGGLE_STREAMING,   drogon::Put);
    METHOD_LIST_END

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetDeviceStatus(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback,
                            const std::string& detectorUuid);

    void processDevicePowerRequest(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback,
                            const std::string& detectorUuid);

    void processToggleStreamingMode(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback,
                            const std::string& detectorUuid,
                            const std::string& streamingMode);

private:
    std::shared_ptr<DetectorCommandProcessor> m_commandEventProcessor;
};

