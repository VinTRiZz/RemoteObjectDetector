#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include "recordobjects.hpp"

namespace drogon::orm {
class DbClient;
using DbClientPtr = std::shared_ptr<DbClient>;
}

namespace Database {

class RecordManager;
using RecordManagerPtr = std::shared_ptr<RecordManager>;

class RecordManager
{
public:
    explicit RecordManager(const std::string& connectionName);
    ~RecordManager();

    void init();

    void setServer(const std::string& address, uint16_t port);
    void setDatabase(const std::string& databaseName);
    void setUser(const std::string& username, const std::string& password);

    /**
     * @brief addRecord Add record into DB
     * @param iValue
     * @return If non-sync mode selected, will return NULL_ID. Otherwise, inserted ID or NULL_ID on error
     */
    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, std::decay_t<T> >, DataObjects::id_t>
    addRecord(T&& iValue) {
        return addRecord(isSync, iValue.getTable(), iValue.toRecord(), iValue.getIdColumn());
    }

    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, std::decay_t<T> >, bool>
    updateRecord(const T& iValue) {
        return updateRecord(isSync, iValue.getTable(),
                            std::string(iValue.getIdColumn().data()) + " = " + (iValue.getId().has_value() ? std::to_string(iValue.getId().value()) : "NULL"),
                            iValue.toRecord());
    }


    template <typename T>
    bool removeRecord(DataObjects::id_t recId, bool isSync = true) {
        T infoRec; // TODO: set table name as static data?
        return removeRecord(isSync, infoRec.getTable(), std::string(infoRec.getIdColumn()) + " = " + (recId.has_value() ? std::to_string(recId.value()) : "NULL"));
    }

    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, std::decay_t<T> >, T>
    getRecord(const DataObjects::id_t recordId) const {
        T res;
        res.initFromRecord(getRecord(isSync, res.getTable(), res.getIdColumn(), recordId));
        return res;
    }

    std::vector<DataObjects::id_t> getAvailableRecords(const std::string_view &tableName, const std::string_view &recordIdColumn) const;

private:
    DataObjects::id_t addRecord(bool isSync, const std::string_view& tableName, const std::map<std::string, recordValue_t>& valueMap, const std::string_view &idColumnName) const;
    bool updateRecord(bool isSync, const std::string_view& tableName, const std::string& whereCondition, const std::map<std::string, recordValue_t>& valueMap);
    bool removeRecord(bool isSync, const std::string_view& tableName, const std::string& whereCondition);
    std::map<std::string, recordValue_t> getRecord(bool isSync, const std::string_view& tableName, const std::string_view& idColumnName, DataObjects::id_t recordId) const;

    drogon::orm::DbClientPtr m_pClient;

    // Connection info
    std::string m_connectionName;
    std::string m_address;
    uint16_t    m_port;
    std::string m_databaseName;
    std::string m_username;
    std::string m_password;

    std::string createConnectionString() const;
    std::string cellDataToString(const recordValue_t& val) const;

    std::string createInsertQuery(const std::string_view &tableName, const std::map<std::string, recordValue_t>& valueMap, const std::string_view &idColumnName) const;
    std::string createUpdateQuery(const std::string_view &tableName, const std::string_view& whereCondition, const std::map<std::string, recordValue_t>& valueMap) const;
};

} // namespace Database
