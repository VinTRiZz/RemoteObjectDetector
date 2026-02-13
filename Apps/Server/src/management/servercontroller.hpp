#pragma once

#include <drogon/drogon.h>

class ServerController : public drogon::HttpController<ServerController, false>
{
public:
    ServerController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ServerController::getStatus,  "/api/server/status", drogon::Get);
        ADD_METHOD_TO(ServerController::rebootServer,      "/api/server/reboot", drogon::Post);
        ADD_METHOD_TO(ServerController::shutdownServer,      "/api/server/shutdown", drogon::Post);
    METHOD_LIST_END

    void getStatus(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void rebootServer(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void shutdownServer(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

