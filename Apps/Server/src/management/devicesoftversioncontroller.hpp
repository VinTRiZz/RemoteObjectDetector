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
        ADD_METHOD_TO(DeviceSoftVersionController::getSoftVersion,  "/api/detector/soft", drogon::Get);
        ADD_METHOD_TO(DeviceSoftVersionController::addVersion,      "/api/detector/soft", drogon::Post);
        ADD_METHOD_TO(DeviceSoftVersionController::setSoftVersion,  "/api/detector/soft", drogon::Put);
        ADD_METHOD_TO(DeviceSoftVersionController::removeVersion,   "/api/detector/soft", drogon::Delete);
    METHOD_LIST_END

    void getSoftVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void addVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void setSoftVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void removeVersion(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void setVersionGetter(std::function<std::string()>&& versionGetter);
    void setUpdater(std::function<bool(const std::string&)>&& versionUpdater);

private:
    ServerEventLogger &m_eventLogger;

    std::function<std::string()> m_versionGetter;
    std::function<bool(const std::string&)> m_versionUpdater;
};

