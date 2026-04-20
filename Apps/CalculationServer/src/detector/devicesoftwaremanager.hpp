#pragma once

#include <string>
#include <map>

#include <ROD/Protocol.h>
#include <ROD/Error.h>

#include "common/servercommon.hpp"

/**
 * @brief The DeviceSoftwareManager class   Manager of device software versions, stored on a server
 */
class DeviceSoftwareManager
{
public:
    DeviceSoftwareManager();
    ~DeviceSoftwareManager();

    std::string getVersionHash(DataObjects::id_t versionId) const;

    bool addVersionFile(const std::string& localFilepath, const std::string &fileMd5Hash);

    std::string getVersionFile(DataObjects::id_t versionId) const;
    std::string getVersionFile(const std::string& versionHash) const;

    bool removeVersion(DataObjects::id_t versionId);
    bool removeVersion(const std::string& versionHash);

    std::vector<std::string> getExistingVersions() const;

private:
    void init();

    mutable DataObjects::Error m_error;
    std::map<std::string, std::string>          m_versionPaths;
    std::map<DataObjects::id_t, std::string>   m_versionHashCache;
};
