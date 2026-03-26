#include "detectorcommandprocessor.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Filework/Common.h>

#include "common/servercommon.hpp"

DetectorCommandProcessor::DetectorCommandProcessor(DeviceSoftwareManager &deviceSoftManager) :
    EventProcessor(),
    m_deviceSoftManager {deviceSoftManager}
{
    setProcessorName("Detector command processor");
}

std::string DetectorCommandProcessor::getCurrentVersion(ServerCommon::id_t deviceId) const
{
    // TODO: Get version
    return {};
}

std::optional<bool> DetectorCommandProcessor::setSoftVersion(ServerCommon::id_t deviceId, ServerCommon::id_t versionId)
{
    auto versionFilename = m_deviceSoftManager.getVersionFile(versionId);
    if (versionFilename.empty()) {
        COMPLOG_WARNING("[DetectorCommandProcessor] Version set failed: Failed to find version file");
        return std::nullopt;
    }

    COMPLOG_WARNING("[DetectorCommandProcessor] Version set failed: NOT IMPLEMENTED");
    return false;
}
