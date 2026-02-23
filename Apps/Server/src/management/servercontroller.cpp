#include "servercontroller.hpp"

#include <Components/Logger/Logger.h>

#include <nlohmann/json.hpp>

ServerController::ServerController() :
    drogon::HttpController<ServerController, false>()
{

}

void ServerController::getStatus(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto pResponse = drogon::HttpResponse::newHttpResponse(drogon::k200OK, drogon::CT_APPLICATION_JSON);

    // Get status
    nlohmann::json response;
    response["uptime"] = m_statusManager.getUptimeSec();
    response["cpu_temp"] = m_statusManager.getCPUCurrentTemperature();
    response["cpu_load"] = m_statusManager.getCPULoad();

    auto spaceInfo = std::filesystem::space(std::filesystem::current_path());
    response["space_total"] = spaceInfo.capacity;
    response["space_available"] = spaceInfo.available;
    response["space_free"] = spaceInfo.free;

    // Response
    pResponse->setStatusCode(drogon::k200OK);
    pResponse->setBody(response.dump());
    callback(pResponse);
}

void ServerController::processPowerRequest(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto actionText = req->getParameter("action");
    if (actionText == "poweroff") {
#ifdef DEBUG_BUILD_MODE
    COMPLOG_DEBUG("POWEROFF PROCEDURE CALLED");
#else
    system("systemctl poweroff");
#endif // DEBUG_BUILD_MODE
    } else if (actionText == "reboot") {
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
