#pragma once

#include <ROD/Protocol.h>

#include <optional>

#include "detector/devicesoftwaremanager.hpp"
#include "common/servercommon.hpp"

class DetectorCommandProcessor : public Protocol::EventProcessor
{
public:
    DetectorCommandProcessor(DeviceSoftwareManager& deviceSoftManager);

    std::string getCurrentVersion(ServerCommon::id_t deviceId) const;
    std::optional<bool> setSoftVersion(ServerCommon::id_t deviceId,
                                       ServerCommon::id_t versionId);

private:
    DeviceSoftwareManager& m_deviceSoftManager;
};

