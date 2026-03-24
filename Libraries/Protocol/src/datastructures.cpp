#include "datastructures.hpp"

#include <nlohmann/json.hpp>

namespace Protocol::Structures
{

/**
 * @brief The ErrorCodes enum Error codes for internal usage
 */
enum ErrorCodes : unsigned
{
    NoError = 0,


    // 0*** --> System errors
    SystemUnknown = 1,


    // 1*** --> Protocol errors
    ProtocolUnknown = 1000,
    ProtocolInvalidVersion,
    ProtocolNoData,
    ProtocolInvalidData,
    ProtocolJsonException,


    // 2*** --> Interface errors
    InterfaceUnknown = 2000,
    InterfaceInvalidHostname,
    InterfaceInvalidPort,


    // 3*** --> Implementation errors
    ImplUnknown = 3000,
    ImplNotImplemented
};

// TODO: Move to database for initialization?
bool initErrorDefinitions() {
    auto& codeMap = Error::getDefinitionInstance();

    codeMap[ErrorCodes::NoError] = "No error";

    // 0*** --> System errors
    codeMap[ErrorCodes::SystemUnknown] = "Unknown system error";


    // 1*** --> Protocol errors
    codeMap[ErrorCodes::ProtocolUnknown]            = "Unknown protocol error";
    codeMap[ErrorCodes::ProtocolInvalidVersion]     = "Invalid version";
    codeMap[ErrorCodes::ProtocolNoData]             = "No data";
    codeMap[ErrorCodes::ProtocolInvalidData]        = "Invalid data";
    codeMap[ErrorCodes::ProtocolJsonException]      = "JSON parse exception";


    // 2*** --> Interface errors
    codeMap[ErrorCodes::InterfaceUnknown]           = "Unknown exchange interface error";
    codeMap[ErrorCodes::InterfaceInvalidHostname]   = "Invalid hostname";
    codeMap[ErrorCodes::InterfaceInvalidPort]       = "Invalid port";


    // 3*** --> Implementation errors
    codeMap[ErrorCodes::ImplUnknown]        = "Unknown internal error";
    codeMap[ErrorCodes::ImplNotImplemented] = "Not implemented";

    return true;
}
static bool initResult = initErrorDefinitions();


void Error::setErrorCode(unsigned int errCode)
{
    m_errorCode = errCode;
}

bool Error::isOk() const
{
    return (m_errorCode == ErrorCodes::NoError);
}

std::string Error::getErrorText() const
{
    auto& definitionMap = getDefinitionInstance();
    auto codeDefinition = definitionMap.find(m_errorCode);
    if (codeDefinition == definitionMap.end()) {
        return {};
    }
    return codeDefinition->second;
}



std::map<unsigned int, std::string> &Error::getDefinitionInstance()
{
    static std::map<unsigned, std::string> codeDefinitions;
    return codeDefinitions;
}

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

}