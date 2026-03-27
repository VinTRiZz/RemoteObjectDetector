#pragma once

#include "recordobjects.hpp"

namespace Database
{

/**
 * @brief The DetectorSystemRecord class detector.system table record
 */
class DetectorSystemRecord : public RecordBase
{
public:
    DetectorSystemRecord();

    // RecordBase interface
    record_t toRecord() const;
    void initFromRecord(const record_t &iRecord);

    void setRegisterDate(int64_t val);
    int64_t getRegisterDate() const;

private:
    int64_t m_registerDate {};
};



/**
 * @brief The DetectorOnlineRecord class detector.online table record
 */
class DetectorOnlineRecord : public RecordBase
{
public:
    DetectorOnlineRecord();

    // RecordBase interface
    record_t toRecord() const;
    void initFromRecord(const record_t &iRecord);

    void setLastOnline(int64_t val);
    int64_t getLastOnlie() const;

    void setTotalOnline(int64_t val);
    int64_t getTotalOnline() const;

private:
    int64_t m_lastOnlineUTC {};
    int64_t m_totalOnline {};
};



/**
 * @brief The DetectorSoftwareRecord class detector.software table record
 */
class DetectorSoftwareRecord : public RecordBase
{
public:
    DetectorSoftwareRecord();

    // RecordBase interface
    record_t toRecord() const;
    void initFromRecord(const record_t &iRecord);

    void setVersionId(DataObjects::id_t id);
    DataObjects::id_t getVersionId() const;

    void setUpdateTime(int64_t uTimeUTC);
    int64_t getUpdateTime() const;

private:
    DataObjects::id_t   m_versionId {DataObjects::NULL_ID};
    int64_t             m_updateTimeUTC {};
};



/**
 * @brief The DetectorInfoRecord class detector.info table record
 * @note All string values converts to/from HEX for database
 */
class DetectorInfoRecord : public RecordBase
{
public:
    DetectorInfoRecord();

    // RecordBase interface
    record_t toRecord() const;
    void initFromRecord(const record_t &iRecord);

    void setDisplayName(const std::string& name);
    std::string getDisplayName() const;

    void setDescription(const std::string& descr);
    std::string getDescription() const;

    void setLocation(const std::string& location);
    std::string getLocation() const;

private:
    std::string m_displayName;
    std::string m_description;
    std::string m_location;
};

}