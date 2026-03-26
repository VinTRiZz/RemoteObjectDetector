#pragma once

#include <string>
#include <map>

#include <ROD/Protocol.h>

#include "common/servercommon.hpp"

/**
 * @brief The DeviceSoftwareManager class   Manager of device software versions, stored on a server
 */
class DeviceSoftwareManager
{
public:
    DeviceSoftwareManager();
    ~DeviceSoftwareManager();

    std::string getVersionHash(ServerCommon::id_t versionId) const;

    bool addVersionFile(const std::string& localFilepath, const std::string &fileMd5Hash);

    std::string getVersionFile(ServerCommon::id_t versionId) const;
    std::string getVersionFile(const std::string& versionHash) const;

    bool removeVersion(ServerCommon::id_t versionId);
    bool removeVersion(const std::string& versionHash);

    std::vector<std::string> getExistingVersions() const;

private:
    void init();

    mutable Protocol::Structures::Error m_error;
    std::map<std::string, std::string>          m_versionPaths;
    std::map<ServerCommon::id_t, std::string>   m_versionHashCache;
};
