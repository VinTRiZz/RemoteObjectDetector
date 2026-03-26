#include "devicescontroller.hpp"

DevicesController::DevicesController(const std::shared_ptr<DetectorCommandProcessor>& detectorCommandProcessor) :
    drogon::HttpController<DevicesController, false>(),
    m_commandEventProcessor {detectorCommandProcessor}
{

}

void DevicesController::processGetDeviceStatus(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string &detectorUuid)
{

}

void DevicesController::processDevicePowerRequest(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback,
    const std::string& detectorUuid)
{

}

void DevicesController::processToggleStreamingMode(
    const drogon::HttpRequestPtr &req,
    ResponseCallback_t &&callback, const std::string &detectorUuid, const std::string &streamingMode)
{

}