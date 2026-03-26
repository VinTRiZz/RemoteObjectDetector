#include "devicestatus.hpp"

#include <nlohmann/json.hpp>

namespace DataObjects {

std::string DeviceStatus::toJson()
{
    nlohmann::json res;
    res["common"]["uptime"] = common.uptime;

    res["cpu"]["temp"] = cpu.temperature;
    res["cpu"]["load"] = cpu.loadPercent;

    res["storage"]["space_total"]       = storage.spaceTotal;
    res["storage"]["space_available"]   = storage.spaceAvailable;
    res["storage"]["space_free"]        = storage.spaceFree;

    m_error.setErrorCode(ErrorCodes::NoError);
    return res.dump();
}

bool DeviceStatus::readJson(const std::string &iString)
{
    try {
        auto statusJson = nlohmann::json::parse(iString);

        common.uptime = statusJson["common"]["uptime"];

        cpu.loadPercent = statusJson["cpu"]["load"];
        cpu.temperature = statusJson["cpu"]["temp"];

        // Funny, that "uintmax_t" in nlohmann::json is double in Qt
        storage.spaceTotal       = statusJson["storage"]["space_total"];
        storage.spaceAvailable   = statusJson["storage"]["space_available"];
        storage.spaceFree        = statusJson["storage"]["space_free"];

        m_error.setErrorCode(ErrorCodes::NoError);
    } catch (nlohmann::json::exception& ex) {
        m_error.setErrorCode(ErrorCodes::ProtocolJsonException);
        return false;
    }
    return true;
}

} // namespace DataObjects
