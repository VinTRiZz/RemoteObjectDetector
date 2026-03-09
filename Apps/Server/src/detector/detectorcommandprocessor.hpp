#pragma once

#include <ROD/Protocol.h>

#include <optional>

class DetectorCommandProcessor : public Protocol::EventProcessor
{
public:
    DetectorCommandProcessor();

    std::string getLastErrorText() const;
    std::string getCurrentVersion() const;

    bool registerSoftVersion(const std::string& softVersionName, const std::string& softVersionHash);
    std::optional<bool> setSoftVersion(const std::string& softVersionName);
    bool removeSoftVersion(const std::string& softVersionName);

private:
    std::string m_lastErrorText {"NO ERROR"};
};

