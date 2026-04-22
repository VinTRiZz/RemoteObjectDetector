#include "detectorconfiguration.hpp"

#include <nlohmann/json.hpp>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

namespace DataObjects
{

std::string DetectorConfiguration::toJson() const
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
    auto hexIfExist = [](auto& iv){ if (iv.has_value()) return nlohmann::json(Encryption::encodeHex(iv.value())); else return nlohmann::json(); };
    res["info"]["name"]         = hexIfExist(info.name);
    res["info"]["description"]  = hexIfExist(info.description);
    res["info"]["location"]     = hexIfExist(info.location);

    res["software"]["version_id"]       = software.versionId;
    res["software"]["updateTimeUTC"]    = software.updateTimeUTC;

    return res.dump();
}

bool DetectorConfiguration::readJson(const std::string &iString)
{
    try
    {
        nlohmann::json iJson = nlohmann::json::parse(iString);

        auto& idj = iJson["system"]["id"];
        system.id               = idj.is_null() ? NULL_ID : id_t{idj.get<id_t::type>()};
        system.registerDateUTC  = iJson["system"]["registerDateUTC"].get<int64_t>();

        online.lastOnlineTimeUTC    = iJson["online"]["lastOnlineTimeUTC"].get<int64_t>();
        online.totalOnlineTime      = iJson["online"]["totalOnlineTime"].get<int64_t>();

        security.token = iJson["security"]["token"].get<std::string>();

        auto dehexIfExist = [](auto& iv) -> ExtraClasses::JOptional<std::string> { if (iv.is_null()) return std::nullopt; else return Encryption::decodeHex(iv); };
        info.name       = dehexIfExist(iJson["info"]["name"]);
        info.description= dehexIfExist(iJson["info"]["description"]);
        info.location   = dehexIfExist(iJson["info"]["location"]);

        software.versionId      = iJson["software"]["version_id"];
        software.updateTimeUTC  = iJson["software"]["updateTimeUTC"];

        m_error.setErrorCode(ErrorCodes::NoError);
        return true;
    } catch (const std::exception& e) {
        COMPLOG_ERROR("Parse error:", e.what());
        m_error.setErrorCode(ErrorCodes::ProtocolJsonException);
    }
    return false;
}

bool DetectorConfiguration::operator ==(const DetectorConfiguration &sconf) const
{
    return system.id                == sconf.system.id &&
           system.registerDateUTC   == sconf.system.registerDateUTC &&
           online.lastOnlineTimeUTC == sconf.online.lastOnlineTimeUTC &&
           online.totalOnlineTime   == sconf.online.totalOnlineTime &&
           security.token           == sconf.security.token &&
           info.name                == sconf.info.name &&
           info.description         == sconf.info.description &&
           info.location            == sconf.info.location &&
           software.versionId       == sconf.software.versionId &&
           software.updateTimeUTC   == sconf.software.updateTimeUTC;
}

}
