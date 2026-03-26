#include "error.hpp"

namespace DataObjects
{

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

}