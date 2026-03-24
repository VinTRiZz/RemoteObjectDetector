#pragma once

#include <string>
#include <map>

namespace Protocol::Structures
{


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


/**
 * @brief The DeviceStatus class Status of a server, detector, etc.
 */
struct DeviceStatus : public ErrorUser
{
    struct CommonInfo
    {
        unsigned long   uptime  {};
    };
    CommonInfo common {};

    struct CPUInfo
    {
        double  temperature  {};
        double  loadPercent  {};
    };
    CPUInfo cpu {};

    struct StorageInfo
    {
        unsigned long   spaceTotal      {};
        unsigned long   spaceAvailable  {};
        unsigned long   spaceFree       {};
    };
    StorageInfo storage {};

    std::string toJson();
    bool readJson(const std::string& iString);
};

}