#pragma once

#include <string>
#include <map>

namespace DataObjects
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


/**
 * @brief The ErrorObject class Class for error describing
 */
class Error
{
public:
    void setErrorCode(unsigned errCode);
    bool isOk() const;
    std::string getErrorText() const;

    /**
     * @brief getDefinitionInstance Get definitions map, defined in DB, for init or use
     * @return
     */
    static std::map<unsigned, std::string>& getDefinitionInstance();

private:
    unsigned m_errorCode {0};
};


/**
 * @brief The ErrorUser class Subclass for error handling
 */
class ErrorUser
{
public:
    bool isValid() const {
        return m_error.isOk();
    }
    std::string getLastErrorString() const {
        return m_error.getErrorText();
    }

protected:
    mutable Error m_error;
};


}