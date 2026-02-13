#include "servercontroller.hpp"

ServerController::ServerController() :
    drogon::HttpController<ServerController, false>()
{

}

void ServerController::getStatus(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void ServerController::rebootServer(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}

void ServerController::shutdownServer(const drogon::HttpRequestPtr &req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{

}
