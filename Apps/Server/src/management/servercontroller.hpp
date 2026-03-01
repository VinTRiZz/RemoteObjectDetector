#pragma once

#include <drogon/drogon.h>

#include <Components/SystemProcessing/StatusManager.h>

class ServerController : public drogon::HttpController<ServerController, false>
{
public:
    ServerController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ServerController::getStatus,              "/api/server/status",   drogon::Get);
        ADD_METHOD_TO(ServerController::processPowerRequest,    "/api/server/power",    drogon::Put);
    METHOD_LIST_END

    void getStatus(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void processPowerRequest(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    SystemProcessing::StatusManager m_statusManager;
};

