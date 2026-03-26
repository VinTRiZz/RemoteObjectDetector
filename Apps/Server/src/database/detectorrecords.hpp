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

}