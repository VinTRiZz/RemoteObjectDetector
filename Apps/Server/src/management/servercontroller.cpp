#include "servercontroller.hpp"

#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>

#include <nlohmann/json.hpp>

ServerController::ServerController() :
    drogon::HttpController<ServerController, false>()
{

}

void ServerController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    // Get status
    Protocol::Structures::DeviceStatus status;

    status.common.uptime = m_statusManager.getUptimeSec();

    status.cpu.temperature = m_statusManager.getCPUCurrentTemperature();
    status.cpu.loadPercent = m_statusManager.getCPULoad();

    auto spaceInfo = std::filesystem::space(std::filesystem::current_path());
    status.storage.spaceTotal       = spaceInfo.capacity;
    status.storage.spaceAvailable   = spaceInfo.available;
    status.storage.spaceFree        = spaceInfo.free;

    // Response
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_APPLICATION_JSON);
    pResponse->setStatusCode(drogon::k200OK);
    pResponse->setBody(status.toJson());
    callback(pResponse);
}

void ServerController::processPowerRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &action)
{
    if (action == Protocol::API::PARAMATER_VALUES::POWER_OFF) {
#ifdef DEBUG_BUILD_MODE
    COMPLOG_DEBUG("POWEROFF PROCEDURE CALLED");
#else
    system("systemctl poweroff");
#endif // DEBUG_BUILD_MODE
    } else if (action == Protocol::API::PARAMATER_VALUES::POWER_REBOOT) {
#ifdef DEBUG_BUILD_MODE
    COMPLOG_DEBUG("REBOT PROCEDURE CALLED");
#else
    system("systemctl reboot");
#endif // DEBUG_BUILD_MODE
    } else {
        auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k400BadRequest, drogon::CT_NONE);
        callback(pResponse);
        return;
    }

    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_NONE);
    callback(pResponse);
}
