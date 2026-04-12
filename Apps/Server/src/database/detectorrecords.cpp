#include "detectorrecords.hpp"

#include <Components/Encryption/Encoding.h>

namespace Database
{

DetectorSystemRecord::DetectorSystemRecord() :
    RecordBase("detector.system")
{

}

record_t DetectorSystemRecord::toRecord() const
{
    auto res = RecordBase::toRecord();
    res["register_date"] = m_registerDate;
    return res;
}

void DetectorSystemRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    m_registerDate = std::get<int64_t>(iRecord.at("register_date"));
}

void DetectorSystemRecord::setRegisterDate(int64_t val)
{
    m_registerDate = val;
}

int64_t DetectorSystemRecord::getRegisterDate() const
{
    return m_registerDate;
}




DetectorOnlineRecord::DetectorOnlineRecord() :
    RecordBase("detector.online", "detector_id")
{

}

record_t DetectorOnlineRecord::toRecord() const
{
    auto res = RecordBase::toRecord();
    res["total"]    = m_totalOnline;
    res["last_utc"] = m_lastOnlineUTC;
    return res;
}

void DetectorOnlineRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    m_totalOnline   = std::get<int64_t>(iRecord.at("total"));
    m_lastOnlineUTC = std::get<int64_t>(iRecord.at("last_utc"));
}

void DetectorOnlineRecord::setLastOnline(int64_t val)
{
    m_lastOnlineUTC = val;
}

int64_t DetectorOnlineRecord::getLastOnlie() const
{
    return m_lastOnlineUTC;
}

void DetectorOnlineRecord::setTotalOnline(int64_t val)
{
    m_totalOnline = val;
}

int64_t DetectorOnlineRecord::getTotalOnline() const
{
    return m_totalOnline;
}




DetectorSoftwareRecord::DetectorSoftwareRecord() :
    RecordBase("detector.software", "detector_id")
{

}

record_t DetectorSoftwareRecord::toRecord() const
{
    auto res = RecordBase::toRecord();
    res["version_id"]   = m_versionId;
    res["update_time_utc"]     = m_updateTimeUTC;
    return res;
}

void DetectorSoftwareRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    try {
        m_versionId = std::get<int64_t>(iRecord.at("version_id"));
        m_updateTimeUTC = std::get<int64_t>(iRecord.at("update_time_utc"));
    } catch (const std::bad_variant_access& ex) {
        m_versionId = DataObjects::NULL_ID;
        m_updateTimeUTC = {};
    }
}

void DetectorSoftwareRecord::setVersionId(DataObjects::id_t id)
{
    m_versionId = id;
}

DataObjects::id_t DetectorSoftwareRecord::getVersionId() const
{
    return m_versionId;
}

void DetectorSoftwareRecord::setUpdateTime(int64_t uTimeUTC)
{
    m_updateTimeUTC = uTimeUTC;
}

int64_t DetectorSoftwareRecord::getUpdateTime() const
{
    return m_updateTimeUTC;
}



DetectorInfoRecord::DetectorInfoRecord() :
    RecordBase("detector.info", "detector_id")
{
    
}

record_t DetectorInfoRecord::toRecord() const
{
    auto res = RecordBase::toRecord();
    res["display_name"] = Encryption::encodeHex(m_displayName);
    res["description"]  = Encryption::encodeHex(m_description);
    res["location"]     = Encryption::encodeHex(m_location);
    return res;
}

void DetectorInfoRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    m_displayName   = Encryption::decodeHex(std::get<std::string>(iRecord.at("display_name")));
    m_description   = Encryption::decodeHex(std::get<std::string>(iRecord.at("description")));
    m_location      = Encryption::decodeHex(std::get<std::string>(iRecord.at("location")));
}

void DetectorInfoRecord::setDisplayName(const std::string &name)
{
    m_displayName = name;
}

std::string DetectorInfoRecord::getDisplayName() const
{
    return m_displayName;
}

void DetectorInfoRecord::setDescription(const std::string &descr)
{
    m_description = descr;
}

std::string DetectorInfoRecord::getDescription() const
{
    return m_description;
}

void DetectorInfoRecord::setLocation(const std::string &location)
{
    m_location = location;
}

std::string DetectorInfoRecord::getLocation() const
{
    return m_location;
}

DetectorConfigRecords_t toRecords(const DataObjects::DetectorConfiguration &detConf) {
    DetectorSystemRecord      recordSys;
    recordSys.setId(detConf.system.id);
    recordSys.setRegisterDate(detConf.system.registerDateUTC);

    DetectorOnlineRecord      recordOnline;
    recordOnline.setTotalOnline(detConf.online.totalOnlineTime);
    recordOnline.setLastOnline(detConf.online.lastOnlineTimeUTC);

    DetectorSoftwareRecord    recordSoftware;
    recordSoftware.setVersionId(detConf.software.versionId);
    recordSoftware.setUpdateTime(detConf.software.updateTimeUTC);

    DetectorInfoRecord        recordInfo;
    recordInfo.setDisplayName(detConf.info.name);
    recordInfo.setDescription(detConf.info.description);
    recordInfo.setLocation(detConf.info.location);

    return std::make_tuple(recordSys, recordOnline, recordSoftware, recordInfo);
}

DataObjects::DetectorConfiguration fromRecords(const DetectorConfigRecords_t &detRecords) {
    DataObjects::DetectorConfiguration res;

    // System data
    auto& systemInfo = std::get<Database::DetectorSystemRecord>(detRecords);
    res.system.id               = systemInfo.getId();
    res.system.registerDateUTC  = systemInfo.getRegisterDate();

    // Online data
    auto& onlineInfo = std::get<Database::DetectorOnlineRecord>(detRecords);
    res.online.totalOnlineTime      = onlineInfo.getTotalOnline();
    res.online.lastOnlineTimeUTC    = onlineInfo.getLastOnlie();

    // Software data
    auto& softwareInfo = std::get<Database::DetectorSoftwareRecord>(detRecords);
    res.software.versionId      = softwareInfo.getVersionId();
    res.software.updateTimeUTC  = softwareInfo.getUpdateTime();

    // Info (display) data
    auto& displayInfo = std::get<Database::DetectorInfoRecord>(detRecords);
    res.info.name           = displayInfo.getDisplayName();
    res.info.description    = displayInfo.getDescription();
    res.info.location       = displayInfo.getLocation();

    return res;
}

}