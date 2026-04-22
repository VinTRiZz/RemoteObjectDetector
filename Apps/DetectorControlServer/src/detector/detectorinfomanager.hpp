#pragma once

#include <Components/Database/SQlite.h>

#include <ROD/DetectorConfiguration.h>

#include "database/recordmanager.hpp"

class DetectorInfoManager
{
public:
    void setRecordManager(const Database::RecordManagerPtr& pManager);

    void updateDetectorsInfo();

    std::vector<DataObjects::id_t> getDetectorList() const;
    std::optional<DataObjects::DetectorConfiguration> getDetectorInfo(const DataObjects::id_t& id);

    DataObjects::id_t addDetector(const DataObjects::DetectorConfiguration& detectorConfig);
    bool updateDetectorData(const DataObjects::DetectorConfiguration& detectorConfig);
    bool removeDetectorData(DataObjects::id_t id);

private:
    Database::RecordManagerPtr m_pRecordManager;
    std::unordered_map<DataObjects::id_t::type, DataObjects::DetectorConfiguration> m_detectors;

    DataObjects::DetectorConfiguration getRecord(DataObjects::id_t id) const;
};

