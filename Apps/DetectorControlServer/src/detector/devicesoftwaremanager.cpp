#include "devicesoftwaremanager.hpp"

#include <Components/Common/DirectoryManager.h>
#include <Components/Logger/Logger.h>

#include <Components/Filework/Common.h>

#include <ROD/Servers/Constants.h>

DeviceSoftwareManager::DeviceSoftwareManager() {
    // TODO: Read-up and insert files
    // m_versionPaths
    // m_versionHashCache
}

DeviceSoftwareManager::~DeviceSoftwareManager()
{

}

std::string DeviceSoftwareManager::getVersionHash(DataObjects::id_t versionId) const
{
    auto targetHash = m_versionHashCache.find(versionId);
    if (targetHash == m_versionHashCache.end()) {
        return {};
    }
    return targetHash->second;
}

bool DeviceSoftwareManager::addVersionFile(const std::string &localFilepath, const std::string& fileMd5Hash)
{
    if (!std::filesystem::exists(localFilepath)) {
        COMPLOG_ERROR("[DeviceSoftwareManager] File not exist:", localFilepath);
        return false;
    }

    auto versionsDir = Common::DirectoryManager::getDirectoryStatic(Constants::DIRTYPE_SOFT_VERSIONS);
    auto copyRes = std::filesystem::copy_file(localFilepath, versionsDir / fileMd5Hash);
    if (!copyRes) {
        COMPLOG_ERROR("[DeviceSoftwareManager] Failed to copy file:", localFilepath);
        return false;
    }

    // TODO: Register version in DB
    // m_versionHashCache
    COMPLOG_WARNING("[DeviceSoftwareManager] REGISTER NOT IMPLEMENTED");


    m_versionPaths[fileMd5Hash] = versionsDir / fileMd5Hash;
    COMPLOG_OK("[DeviceSoftwareManager] Version registered:", fileMd5Hash);
    return false;
}

std::string DeviceSoftwareManager::getVersionFile(DataObjects::id_t versionId) const
{
    return getVersionFile(getVersionHash(versionId));
}

std::string DeviceSoftwareManager::getVersionFile(const std::string &versionHash) const
{
    if (versionHash.empty()) {
        COMPLOG_ERROR("[DeviceSoftwareManager] Empty hash");
        return {};
    }
    auto resPath = m_versionPaths.find(versionHash);
    if (resPath == m_versionPaths.end()) {
        return {};
    }
    return resPath->second;
}

bool DeviceSoftwareManager::removeVersion(DataObjects::id_t versionId)
{
    return removeVersion(getVersionHash(versionId));
}

bool DeviceSoftwareManager::removeVersion(const std::string &versionHash)
{
    if (versionHash.empty()) {
        COMPLOG_ERROR("[DeviceSoftwareManager] Empty hash");
        return false;
    }

    auto versionFile = getVersionFile(versionHash);
    if (std::filesystem::exists(versionFile)) {
        auto res = std::filesystem::remove(versionFile);
        if (res) {
            COMPLOG_INFO("[DeviceSoftwareManager] Removed version file of:", versionHash);
        }
        COMPLOG_WARNING("[DeviceSoftwareManager] Failed to remove version file of:", versionHash);
        return false;
    }
    COMPLOG_WARNING("[DeviceSoftwareManager] Version not exist, not deleted:", versionHash);
    return false;
}

std::vector<std::string> DeviceSoftwareManager::getExistingVersions() const
{
    std::vector<std::string> res;

    res.reserve(m_versionHashCache.size());
    for (auto& vers : m_versionHashCache) {
        res.push_back(vers.second);
    }

    return res;
}
