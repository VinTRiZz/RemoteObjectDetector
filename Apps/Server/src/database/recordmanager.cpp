#include "recordmanager.hpp"

#include <drogon/drogon.h>

#include <Components/Logger/Logger.h>

namespace Database {

// TODO: Move to common?
static std::vector<record_t> resultToRecords(const drogon::orm::Result& execResult) {

    std::map<int, int> columnTypes; // For cast easier

    std::vector<record_t> res;
    for (auto& row : execResult) {
        record_t record;
        for (size_t i = 0; i < row.size(); ++i) {
            // Use deduced type
            if (columnTypes[i] != 0) {
                switch (columnTypes[i])
                {
                case 1:
                    row[i].isNull() ? record[execResult.columnName(i)] = int64_t{} : record[execResult.columnName(i)] = row[i].as<int64_t>();
                    break; // Integer

                case 3:
                    row[i].isNull() ? record[execResult.columnName(i)] = double{} : record[execResult.columnName(i)] = row[i].as<double>();
                    break; // Double

                case 4:
                    row[i].isNull() ? record[execResult.columnName(i)] = std::string{} : record[execResult.columnName(i)] = row[i].as<std::string>();
                    break; // String
                }
                continue;
            }

            if (row[i].isNull()) {
                record[execResult.columnName(i)] = {};
                continue;
            }

            // Deduce type
            try {
                record[execResult.columnName(i)] = row[i].as<int64_t>();
                columnTypes[i] = 1;
            } catch (const std::invalid_argument& ex) {
                try {
                    record[execResult.columnName(i)] = row[i].as<double>();
                    columnTypes[i] = 3;
                } catch (...) {
                    record[execResult.columnName(i)] = row[i].as<std::string>();
                    columnTypes[i] = 4;
                }
            }
        }
        res.push_back(record);
    }
    return res;
}

RecordManager::RecordManager(const std::string &connectionName) :
    m_connectionName{ connectionName }
{

}

RecordManager::~RecordManager()
{

}

void RecordManager::init()
{
    m_pClient = drogon::orm::DbClient::newPgClient(createConnectionString(), 2);
}

void RecordManager::setServer(const std::string &address, uint16_t port)
{
    m_address = address;
    m_port = port;
}

void RecordManager::setDatabase(const std::string &databaseName)
{
    m_databaseName = databaseName;
}

void RecordManager::setUser(const std::string &username, const std::string &password)
{
    m_username = username;
    m_password = password;
}

int RecordManager::removeRecord(const std::string& tableName, const std::string& whereCondition)
{
    std::string query = "DELETE FROM " + tableName + (whereCondition.empty() ? "" : std::string(" WHERE ") + whereCondition);

    try {
        m_pClient->execSqlSync(query);
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
        return false;
    }
    return true;
}

std::vector<DataObjects::id_t> RecordManager::getAvailableRecords(const std::string& tableName, const std::string &recordIdColumn) const
{
    std::vector<DataObjects::id_t> res;
    try {
        auto execRes = m_pClient->execSqlSync(std::string("SELECT ") + recordIdColumn + " FROM " + tableName);
        auto rows = resultToRecords(execRes);
        for (auto& r : rows) {
            res.push_back(std::get<int64_t>(r.at(recordIdColumn)));
        }
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
        return {};
    }
    return res;
}

bool RecordManager::addRecord(bool isSync, const std::string &tableName, const std::map<std::string, recordValue_t> &valueMap) const
{
    if (isSync) {
        try {
            m_pClient->execSqlSync(createInsertQuery(tableName, valueMap));
        } catch (const drogon::orm::DrogonDbException& ex) {
            COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
            return false;
        }
    } else {
        m_pClient->execSqlAsync(createInsertQuery(tableName, valueMap), [](const drogon::orm::Result& res){
            // TODO: Process?
        }, [](const drogon::orm::DrogonDbException& ex){
                                    COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
                                });
        return true;
    }
    return false;
}

bool RecordManager::updateRecord(bool isSync, const std::string& tableName, const std::string& whereCondition, const std::map<std::string, recordValue_t>& valueMap)
{
    if (isSync) {
        try {
            m_pClient->execSqlSync(createUpdateQuery(tableName, whereCondition, valueMap));
        } catch (const drogon::orm::DrogonDbException& ex) {
            COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
            return false;
        }
    } else {
        m_pClient->execSqlAsync(createUpdateQuery(tableName, whereCondition, valueMap), [](const drogon::orm::Result& res){
            // TODO: Process?
        }, [](const drogon::orm::DrogonDbException& ex){
                                    COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
                                });
        return true;
    }
    return false;
}

std::map<std::string, recordValue_t> RecordManager::getRecord(bool isSync, const std::string_view &tableName, const std::string_view &idColumnName, DataObjects::id_t recordId) const
{
    std::string query = std::string("SELECT * FROM ") + tableName.data() + " WHERE " + idColumnName.data() + " = " + std::to_string(recordId);
    try {
        auto res = m_pClient->execSqlSync(query);
        auto records = resultToRecords(res);

        if (!records.empty()) {
            return records.front();
        }
    } catch (const drogon::orm::DrogonDbException& ex) {
        COMPLOG_ERROR("[RecordManager] ASYNC Record exec error:", ex.base().what());
        return {};
    }
    return {};
}

std::string RecordManager::createConnectionString() const
{
    // "host=127.0.0.1 port=5432 dbname=test user=user password=pass"
    std::string connString = "host=" + m_address +
                             " port=" + std::to_string(m_port) +
                             " dbname=" + m_databaseName +
                             " user=" + m_username +
                             " password=" + m_password;
    return connString;
}

std::string RecordManager::cellDataToString(const recordValue_t &val) const
{
    return std::visit([](auto& v) -> std::string {
        using valueType_t = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<valueType_t, std::string>) {
            return v;
        } else
            if constexpr (std::is_same_v<valueType_t, DataObjects::id_t>) {
                return std::to_string(v);
            } else
                if constexpr (std::is_same_v<valueType_t, double>) {
                    return std::to_string(v);
                }
        return {};
    }, val);
}

std::string RecordManager::createInsertQuery(const std::string_view &tableName, const std::map<std::string, recordValue_t> &valueMap) const
{
    std::string colsQuery;
    std::string valuesQuery;
    for (auto& [colName, colValue] : valueMap) {
        colsQuery += colName + ",";
        valuesQuery += cellDataToString(colValue) + ",";
    }
    colsQuery.pop_back();
    valuesQuery.pop_back();

    return std::string("INSERT INTO ") + tableName.data() + " (" + colsQuery + ") VALUES (" + valuesQuery + ")";
}

std::string RecordManager::createUpdateQuery(const std::string_view &tableName, const std::string_view &whereCondition, const std::map<std::string, recordValue_t> &valueMap) const
{
    std::string query("UPDATE ");
    query += tableName.data();
    query += " SET ";

    for (auto& [colName, colValue] : valueMap) {
        query += colName + "=" + cellDataToString(colValue) + ",";
    }
    query.pop_back();
    query += (whereCondition.empty() ? "" : std::string(" WHERE ") + whereCondition.data());

    return query;
}



} // namespace Database
