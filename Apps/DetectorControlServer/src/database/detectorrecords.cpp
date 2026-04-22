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
    record_t res; // Ignore ID changing, it's not allowed
    res["register_date"] = m_registerDate;
    return res;
}

void DetectorSystemRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    m_registerDate = std::get<int64_t>(iRecord.at("register_date").value_or(0));
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
    m_totalOnline   = std::get<int64_t>(iRecord.at("total").value_or(0));
    m_lastOnlineUTC = std::get<int64_t>(iRecord.at("last_utc").value_or(0));
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
    res["version_id"]           = m_versionId;
    res["update_time_utc"]      = m_updateTimeUTC;
    return res;
}

void DetectorSoftwareRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    try {
        m_versionId = std::get<int64_t>(iRecord.at("version_id").value_or(0));
        m_updateTimeUTC = std::get<int64_t>(iRecord.at("update_time_utc").value_or(0));
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
    auto hexIfExist = [](auto& iv) -> recordValue_t {
        if (iv.has_value()) {
            return std::string("HEX-") + Encryption::encodeHex(iv.value());
        }
        else
            return recordValue_t{};
    };
    res["display_name"] = hexIfExist(m_displayName);
    res["description"]  = hexIfExist(m_description);
    res["location"]     = hexIfExist(m_location);
    return res;
}

void DetectorInfoRecord::initFromRecord(const record_t &iRecord)
{
    RecordBase::initFromRecord(iRecord);
    auto dehexIfExist = [](auto& iv) -> ExtraClasses::JOptional<std::string> {
        if (iv.has_value() && !std::holds_alternative<std::monostate>(iv.value())) {
            auto vstr = std::get<std::string>(iv.value());
            vstr.erase(vstr.begin(), vstr.begin() + 4); // HEX-
            return Encryption::decodeHex(vstr);
        }
        else
            return std::nullopt;
    };
    m_displayName   = dehexIfExist(iRecord.at("display_name"));
    m_description   = dehexIfExist(iRecord.at("description"));
    m_location      = dehexIfExist(iRecord.at("location"));
}

void DetectorInfoRecord::setDisplayName(const std::string &name)
{
    m_displayName = name;
}

std::string DetectorInfoRecord::getDisplayName() const
{
    return m_displayName.value_or("");
}

void DetectorInfoRecord::setDescription(const std::string &descr)
{
    m_description = descr;
}

std::string DetectorInfoRecord::getDescription() const
{
    return m_description.value_or("");
}

void DetectorInfoRecord::setLocation(const std::string &location)
{
    m_location = location;
}

std::string DetectorInfoRecord::getLocation() const
{
    return m_location.value_or("");
}

DetectorConfigRecords_t toRecords(const DataObjects::DetectorConfiguration &detConf) {
    DetectorSystemRecord recordSys;
    recordSys.setId(detConf.system.id);
    recordSys.setRegisterDate(detConf.system.registerDateUTC);

    DetectorOnlineRecord recordOnline;
    recordOnline.setId(detConf.system.id);
    recordOnline.setTotalOnline(detConf.online.totalOnlineTime);
    recordOnline.setLastOnline(detConf.online.lastOnlineTimeUTC);

    DetectorSoftwareRecord recordSoftware;
    recordSoftware.setId(detConf.system.id);
    recordSoftware.setVersionId(detConf.software.versionId);
    recordSoftware.setUpdateTime(detConf.software.updateTimeUTC);

    DetectorInfoRecord recordInfo;
    recordInfo.setId(detConf.system.id);
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