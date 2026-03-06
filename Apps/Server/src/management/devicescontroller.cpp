#include "devicescontroller.hpp"

DevicesController::DevicesController(Protocol::EventProcessor &commandEventProcessor) :
    drogon::HttpController<DevicesController, false>(),
    m_commandEventProcessor {commandEventProcessor}
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
