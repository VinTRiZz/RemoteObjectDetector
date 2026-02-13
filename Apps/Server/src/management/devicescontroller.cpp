#include "devicescontroller.hpp"

DevicesController::DevicesController(ServerEventLogger& eventLogger) :
    drogon::HttpController<DevicesController, false>(),
    m_eventLogger {eventLogger}
{

}

void DevicesController::getDeviceStatus(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DevicesController::rebootDevice(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DevicesController::poweroffDevice(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void DevicesController::setStreamingMode(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}
