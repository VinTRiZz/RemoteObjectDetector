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
        m_detectors.emplace(id, getRecord(id));
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
    auto targetIt = m_detectors.find(id);
    if (targetIt == m_detectors.end()) {
        return std::nullopt;
    }
    return targetIt->second;
}

bool DetectorInfoManager::updateDetectorData(const DataObjects::DetectorConfiguration &detectorConfig)
{
    auto targetIt = m_detectors.find(detectorConfig.system.id);
    if (targetIt == m_detectors.end()) {
        return false;
    }

    bool isSucceed = false;
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
    auto remRes = m_pRecordManager->removeRecord<Database::DetectorSystemRecord>(id);
    if (remRes) {
        m_detectors.erase(id);
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

bool DetectorInfoManager::save(const DataObjects::DetectorConfiguration &detectorConfig)
{
    bool isSucceed = false;
    auto saveRecord = [&isSucceed, this](auto& rec){
        if (!isSucceed) {
            return;
        }
        isSucceed = m_pRecordManager->addRecord(rec);
    };
    auto saveConfig = [updateRecord = std::move(saveRecord)](auto&&... records) -> void {
        (updateRecord(records), ...);
    };

    auto detectorInfo = Database::toRecords(detectorConfig);
    std::apply(saveConfig, detectorInfo);
    if (isSucceed) {
        m_detectors[detectorConfig.system.id] = detectorConfig;
    }
    return isSucceed;
}
