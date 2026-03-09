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
        ADD_METHOD_TO(DevicesController::getDeviceStatus,   "/api/detector/status",   drogon::Get);
        ADD_METHOD_TO(DevicesController::rebootDevice,      "/api/detector/power",    drogon::Put);
        ADD_METHOD_TO(DevicesController::setStreamingMode,  "/api/detector/stream",   drogon::Put); // Start / stop streaming
    METHOD_LIST_END

    void getDeviceStatus(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void rebootDevice(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void poweroffDevice(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void setStreamingMode(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    std::shared_ptr<DetectorCommandProcessor> m_commandEventProcessor;
};

