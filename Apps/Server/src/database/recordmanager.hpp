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

    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, T>, bool>
    addRecord(T&& iValue) {
        return addRecord(isSync, iValue.getTable(), iValue.toRecord());
    }

    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, T>, bool>
    updateRecord(T&& iValue) {

    }

    int removeRecord(const std::string &tableName, const std::string &whereCondition);

    template <bool isSync = true, typename T>
    std::enable_if_t<std::is_base_of_v<RecordBase, T>, T>
    getRecord(const DataObjects::id_t recordId) const {
        T res;
        res.initFromRecord(getRecord(isSync, res.getTable(), res.getIdColumn(), recordId));
        return res;
    }

    std::vector<DataObjects::id_t> getAvailableRecords(const std::string &tableName, const std::string& recordIdColumn = "id") const;

private:
    bool addRecord(bool isSync, const std::string& tableName, const std::map<std::string, recordValue_t>& valueMap) const;
    bool updateRecord(bool isSync, const std::string& tableName, const std::string& whereCondition, const std::map<std::string, recordValue_t>& valueMap);
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

    std::string createInsertQuery(const std::string_view &tableName, const std::map<std::string, recordValue_t>& valueMap) const;
    std::string createUpdateQuery(const std::string_view &tableName, const std::string_view& whereCondition, const std::map<std::string, recordValue_t>& valueMap) const;
};

} // namespace Database
