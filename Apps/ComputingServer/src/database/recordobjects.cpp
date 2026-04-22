#include "recordobjects.hpp"

namespace Database {

RecordBase::RecordBase(const std::string &tableName, const std::string &idColumn) :
    m_table {tableName},
    m_idColumnName {idColumn}
{

}

std::string_view RecordBase::getTable() const
{
    return m_table;
}

std::string_view RecordBase::getIdColumn() const
{
    return m_idColumnName;
}

record_t RecordBase::toRecord() const
{
    record_t res;
    res[m_idColumnName] = m_id;
    return res;
}

void RecordBase::initFromRecord(const record_t &iRecord)
{
    auto& idVal = iRecord.at(m_idColumnName);
    if (idVal.has_value()) {
        m_id = std::get<int64_t>(idVal.value());
    } else {
        m_id = DataObjects::NULL_ID;
    }
}

void RecordBase::setId(DataObjects::id_t id)
{
    m_id = id;
}

DataObjects::id_t RecordBase::getId() const
{
    return m_id;
}

} // namespace Database
