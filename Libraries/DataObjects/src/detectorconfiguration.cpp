#include "detectorconfiguration.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>

namespace DataObjects
{

std::string DetectorConfiguration::toJson()
{
    nlohmann::json res;

    // Сериализация SystemInfo
    res["system"]["id"] = system.id;
    res["system"]["registerDateUTC"] = system.registerDateUTC;

    // Сериализация OnlineInfo
    res["online"]["lastOnlineTimeUTC"] = online.lastOnlineTimeUTC;
    res["online"]["totalOnlineTime"] = online.totalOnlineTime;

    // Сериализация Security
    res["security"]["token"] = security.token;

    // Сериализация Info
    res["info"]["name"] = info.name;
    res["info"]["location"] = info.location;

    return res.dump();
}

bool DetectorConfiguration::readJson(const std::string &iString)
{
    try
    {
        nlohmann::json iJson = nlohmann::json::parse(iString);

        system.id               = iJson["system"]["id"].get<id_t>();
        system.registerDateUTC  = iJson["system"]["registerDateUTC"].get<int64_t>();

        online.lastOnlineTimeUTC    = iJson["online"]["lastOnlineTimeUTC"].get<int64_t>();
        online.totalOnlineTime      = iJson["online"]["totalOnlineTime"].get<int64_t>();

        security.token = iJson["security"]["token"].get<std::string>();

        info.name       = iJson["info"]["name"].get<std::string>();
        info.location   = iJson["info"]["location"].get<std::string>();
        m_error.setErrorCode(ErrorCodes::NoError);
        return true;
    } catch (const std::exception& e) {
        COMPLOG_ERROR("Parse error:", e.what());
        m_error.setErrorCode(ErrorCodes::ProtocolJsonException);
    }
    return false;
}


}
