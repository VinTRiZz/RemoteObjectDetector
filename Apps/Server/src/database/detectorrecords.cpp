#include "detectorrecords.hpp"

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

}