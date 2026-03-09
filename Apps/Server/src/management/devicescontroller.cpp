#include "devicescontroller.hpp"

DevicesController::DevicesController(const std::shared_ptr<DetectorCommandProcessor>& detectorCommandProcessor) :
    drogon::HttpController<DevicesController, false>(),
    m_commandEventProcessor {detectorCommandProcessor}
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
