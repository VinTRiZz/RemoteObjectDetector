#include "servereventlogger.hpp"

#include <time.h>
#include <sstream>
#include <iomanip>

#include <Components/Encryption/Encoding.h>
#include <Components/Logger/Logger.h>

ServerEventLogger::ServerEventLogger(Database::SQLiteDatabase &serverDb) :
    m_serverDb(serverDb)
{

}

ServerEventLogger::~ServerEventLogger()
{

}

bool ServerEventLogger::init()
{
    Database::SQLiteTable logTable(m_serverDb);
    logTable.setTable("event_log");
    if (!logTable.isTableExist()) {

        std::list<Database::SQLiteTable::ColumnInfo> cols;

        Database::SQLiteTable::ColumnInfo col;
        col.name = "id";
        col.canBeNull = "false";
        col.isPrimaryKey = true;
        col.type = Database::ColumnType::CT_INTEGER;
        cols.push_back(col);

        col = {};
        col.name = "timestamp";
        col.canBeNull = "false";
        col.type = Database::ColumnType::CT_TEXT;
        cols.push_back(col);

        col = {};
        col.name = "type";
        col.canBeNull = "false";
        col.type = Database::ColumnType::CT_TEXT;
        cols.push_back(col);

        col = {};
        col.name = "data";
        col.type = Database::ColumnType::CT_TEXT;
        cols.push_back(col);

        return logTable.create(cols);
    }
    return true;
}

Database::SQLiteDatabase &ServerEventLogger::getServerDb()
{
    return m_serverDb;
}

void ServerEventLogger::logEvent(ServerCommon::EventType evType, const std::string &evInfo)
{
    Database::SQLiteTable logTable(m_serverDb);
    logTable.setTable("event_log");
    logTable.addRow({ {"timestamp", getTimestamp()}, {"type", ServerCommon::toString(evType)}, {"data", Encryption::encodeHex(evInfo)} });
    COMPLOG_INFO("Server event:", ServerCommon::toString(evType), !evInfo.empty() ? (std::string(": ") + evInfo) : std::string());
}

std::string ServerEventLogger::getTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm;

    // Use localtime_r for thread safety (POSIX)
    localtime_r(&now_c, &now_tm);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S") // Unified format
        << '.' << std::setfill('0') << std::setw(3)
        << (now_ms.time_since_epoch().count() % 1000);

    return oss.str();
}
