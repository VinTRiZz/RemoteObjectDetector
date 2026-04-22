#pragma once

#include <ROD/Exchange/Events.h>

#include <optional>

#include "detector/devicesoftwaremanager.hpp"
#include <ROD/Servers/EventProcessor.h>

class DetectorCommandProcessor : public Protocol::EventProcessor
{
public:
    DetectorCommandProcessor(DeviceSoftwareManager& deviceSoftManager);

    std::string getCurrentVersion(DataObjects::id_t deviceId) const;
    std::optional<bool> setSoftVersion(DataObjects::id_t deviceId,
                                       DataObjects::id_t versionId);

private:
    DeviceSoftwareManager& m_deviceSoftManager;
};

