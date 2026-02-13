#pragma once

#include <drogon/drogon.h>

#include "../endpoint/servereventlogger.hpp"

/**
 * @brief The DevicesController class Контроллер, отвечающий за управление детекторами
 */
class DevicesController : public drogon::HttpController<DevicesController, false>
{
public:
    DevicesController(ServerEventLogger& eventLogger);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DevicesController::getDeviceStatus,   "/api/device/status",   drogon::Get);
        ADD_METHOD_TO(DevicesController::rebootDevice,      "/api/device/reboot",   drogon::Put);
        ADD_METHOD_TO(DevicesController::poweroffDevice,    "/api/device/shutdown", drogon::Put);
        ADD_METHOD_TO(DevicesController::setStreamingMode,  "/api/device/stream",   drogon::Put); // Start / stop streaming
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
    ServerEventLogger& m_eventLogger;
};

