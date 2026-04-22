#include "detectorinfomanager.hpp"

#include <drogon/drogon.h>

#include <Components/Logger/Logger.h>

#include "database/detectorrecords.hpp"

void DetectorInfoManager::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void DetectorInfoManager::updateDetectorsInfo()
{
    m_detectors.clear();
    Database::DetectorSystemRecord tmpr;
    auto idVect = m_pRecordManager->getAvailableRecords(tmpr.getTable(), tmpr.getIdColumn());
    m_detectors.reserve(idVect.size());
    for (auto id : idVect) {
        if (!id.has_value()) {
            continue;
        }
        m_detectors.emplace(id.value(), getRecord(id));
    }
    COMPLOG_INFO("Loaded info about", m_detectors.size(), "detectors");
}

std::vector<DataObjects::id_t> DetectorInfoManager::getDetectorList() const
{
    std::vector<DataObjects::id_t> res;
    res.reserve(m_detectors.size());
    for (auto& [id, info] : m_detectors) {
        res.push_back(id);
    }
    return res;
}

std::optional<DataObjects::DetectorConfiguration> DetectorInfoManager::getDetectorInfo(const DataObjects::id_t &id)
{
    if (!id.has_value()) {
        return std::nullopt;
    }
    auto targetIt = m_detectors.find(id.value());
    if (targetIt == m_detectors.end()) {
        return std::nullopt;
    }
    return targetIt->second;
}

DataObjects::id_t DetectorInfoManager::addDetector(const DataObjects::DetectorConfiguration &detectorConfig)
{
    // Get ID of inserted one
    auto detInfoCopy = detectorConfig;
    auto detectorInfoCopy = Database::toRecords(detInfoCopy);
    auto createdId = m_pRecordManager->addRecord(std::get<Database::DetectorSystemRecord>(detectorInfoCopy));
    if (!createdId.has_value()) {
        return std::nullopt;
    }
    detInfoCopy.system.id = createdId.value();
    m_detectors[createdId.value()] = detInfoCopy;

    // Update inserted values (they are inserted by triggers)
    return (updateDetectorData(detInfoCopy) ? createdId : std::nullopt);
}

bool DetectorInfoManager::updateDetectorData(const DataObjects::DetectorConfiguration &detectorConfig)
{
    auto targetIt = m_detectors.find(detectorConfig.system.id);
    if (targetIt == m_detectors.end()) {
        return false;
    }

    bool isSucceed = true;
    auto updateRecord = [&isSucceed, this](auto& rec){
        if (!isSucceed) {
            return;
        }
        isSucceed = m_pRecordManager->updateRecord(rec);
    };
    auto updateConfig = [updateRecord = std::move(updateRecord)](auto&&... records) -> void {
        (updateRecord(records), ...);
    };

    auto detectorInfo = Database::toRecords(detectorConfig);
    std::apply(updateConfig, detectorInfo);
    if (isSucceed) {
        targetIt->second = detectorConfig;
    }
    return isSucceed;
}

bool DetectorInfoManager::removeDetectorData(DataObjects::id_t id)
{
    if (!id.has_value()) {
        return false;
    }
    auto remRes = m_pRecordManager->removeRecord<Database::DetectorSystemRecord>(id);
    if (remRes) {
        m_detectors.erase(id.value());
    }
    return remRes;
}

DataObjects::DetectorConfiguration DetectorInfoManager::getRecord(DataObjects::id_t id) const
{
    return Database::fromRecords(std::make_tuple(
        m_pRecordManager->getRecord<true, Database::DetectorSystemRecord>(id),
        m_pRecordManager->getRecord<true, Database::DetectorOnlineRecord>(id),
        m_pRecordManager->getRecord<true, Database::DetectorSoftwareRecord>(id),
        m_pRecordManager->getRecord<true, Database::DetectorInfoRecord>(id)
        ));
}
