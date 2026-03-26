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
    auto idVect = m_pRecordManager->getAvailableRecords("detector.system");
    m_detectors.reserve(idVect.size());
    for (auto id : idVect) {
        m_detectors.emplace(id, readById(id));
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

DataObjects::DetectorConfiguration DetectorInfoManager::readById(DataObjects::id_t id)
{
    DataObjects::DetectorConfiguration res;

    // System data
    auto systemInfo = m_pRecordManager->getRecord<true, Database::DetectorSystemRecord>(id);
    res.system.id               = systemInfo.getId();
    res.system.registerDateUTC  = systemInfo.getRegisterDate();

    // Online data
    auto onlineInfo = m_pRecordManager->getRecord<true, Database::DetectorOnlineRecord>(id);
    res.online.totalOnlineTime      = onlineInfo.getTotalOnline();
    res.online.lastOnlineTimeUTC    = onlineInfo.getLastOnlie();

    return res;
}

bool DetectorInfoManager::save(const DataObjects::DetectorConfiguration &configData)
{
    return false;
}
