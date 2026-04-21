#pragma once

#include <string>
#include <variant>
#include <map>

#include <ROD/Types.h>

namespace Database {

// Common
using recordValue_t = std::variant<std::string, int64_t, double>;
using record_t = std::map<std::string, recordValue_t>;

/**
 * @brief The RecordBase class Basic class for converting from/to DB records
 */
class RecordBase {
    std::string m_table;
public:
    explicit RecordBase(const std::string& tableName, const std::string& idColumn = "id");

    std::string_view getTable() const;
    std::string_view getIdColumn() const;

    virtual record_t toRecord() const;
    virtual void initFromRecord(const record_t& iRecord);

    void setId(DataObjects::id_t id);
    DataObjects::id_t getId() const;

private:
    std::string m_idColumnName;
    DataObjects::id_t m_id {DataObjects::NULL_ID};
};

} // namespace Database
