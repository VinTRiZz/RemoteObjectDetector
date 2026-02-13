#pragma once

#include <drogon/drogon.h>
#include "../endpoint/servereventlogger.hpp"

/**
 * @brief The DeviceSoftVersionController class Контроллер, отвечающий за версию ПО, установленную на устройствах
 */
class DeviceSoftVersionController : public drogon::HttpController<DeviceSoftVersionController, false>
{
public:
    DeviceSoftVersionController(ServerEventLogger &eventLogger);

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DeviceSoftVersionController::getSoftVersion,  "/api/soft", drogon::Get);
        ADD_METHOD_TO(DeviceSoftVersionController::addVersion,      "/api/soft", drogon::Post);
        ADD_METHOD_TO(DeviceSoftVersionController::setSoftVersion,  "/api/soft", drogon::Put);
        ADD_METHOD_TO(DeviceSoftVersionController::removeVersion,   "/api/soft", drogon::Delete);
    METHOD_LIST_END

    void getSoftVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void addVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void setSoftVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void removeVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    ServerEventLogger &m_eventLogger;
};

