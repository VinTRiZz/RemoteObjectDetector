#include "detectorcommandprocessor.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include "../endpoint/servercommon.hpp"

DetectorCommandProcessor::DetectorCommandProcessor() :
    EventProcessor()
{
    setProcessorName("Detector command processor");
}

std::string DetectorCommandProcessor::getLastErrorText() const
{
    return m_lastErrorText;
}

std::string DetectorCommandProcessor::getCurrentVersion() const
{
    // TODO: Get version
    return "NULL";
}

bool DetectorCommandProcessor::registerSoftVersion(const std::string &softVersionName, const std::string &softVersionHash)
{
    // TODO: add to database
    m_lastErrorText = "NOT IMPLEMENTED";
    return false;
}

std::optional<bool> DetectorCommandProcessor::setSoftVersion(const std::string &softVersionName)
{
    std::string versionFilename;
    auto versionsDir = Common::DirectoryManager::getDirectoryStatic(ServerCommon::DIRTYPE_SOFT_VERSIONS);
    for (auto& filename : Filework::Common::getContentNames(versionsDir)) {
        auto versionString = filename;
        versionString.erase(versionString.find_first_of("_"), -1);
        if (versionString != softVersionName) {
            continue;
        }
        versionFilename = (versionsDir / filename);
        break;
    }

    if (versionFilename.empty()) {
        m_lastErrorText = "DID NOT FOUND VERSION FILE";
        return std::nullopt;
    }

    COMPLOG_WARNING("Checks complete, but failed to set soft version (not implemented)");
    m_lastErrorText = "NOT IMPLEMENTED";
    return false;
}

bool DetectorCommandProcessor::removeSoftVersion(const std::string &softVersionName)
{
    auto versionsDir = Common::DirectoryManager::getDirectoryStatic(ServerCommon::DIRTYPE_SOFT_VERSIONS);
    for (auto& filename : Filework::Common::getContentNames(versionsDir)) {
        auto versionString = filename;
        versionString.erase(versionString.find_first_of("_"), -1);
        if (versionString != softVersionName) {
            continue;
        }
        std::filesystem::remove(versionsDir / filename);

        auto versionHash = filename;
        versionHash.erase(0, versionHash.find_last_of("_") + 1);
        return true;
    }
    m_lastErrorText = "NOT FOUND VERSION FILE";
    return false;
}
