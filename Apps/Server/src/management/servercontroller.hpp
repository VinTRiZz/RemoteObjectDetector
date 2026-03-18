#pragma once

#include <drogon/drogon.h>

#include <Components/SystemProcessing/StatusManager.h>

#include <ROD/Protocol.h>

class ServerController : public drogon::HttpController<ServerController, false>
{
public:
    ServerController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ServerController::processGetStatus,       Protocol::API::DROGON::SERVER_STATUS,   drogon::Get);
        ADD_METHOD_TO(ServerController::processPowerRequest,    Protocol::API::DROGON::SERVER_POWER,    drogon::Put);
    METHOD_LIST_END

    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;

    void processGetStatus(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback);

    void processPowerRequest(const drogon::HttpRequestPtr &req,
                            ResponseCallback_t &&callback,
                            const std::string& action);

private:
    SystemProcessing::StatusManager m_statusManager;
};

