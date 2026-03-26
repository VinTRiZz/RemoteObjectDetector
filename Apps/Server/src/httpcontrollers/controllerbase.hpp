#pragma once

#include <drogon/drogon.h>

class ControllerBase
{
public:

protected:
    using ResponseCallback_t = std::function<void(const drogon::HttpResponsePtr&)>;
    void sendTextMessage(drogon::HttpStatusCode status, const std::string& textMsg, ResponseCallback_t&& cbk) const;
    void sendJsonMessage(drogon::HttpStatusCode status, const std::string& jsonData, ResponseCallback_t&& cbk) const;
};
