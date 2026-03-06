#pragma once

#include <Components/Database/SQlite.h>

#include <ROD/Protocol.h>

/**
 * @brief The ServerEventLogger class Мастер записи событий в лог-бд для панели управления
 */
class ServerEventLogger
{
public:
    ServerEventLogger(Database::SQLiteDatabase& serverDb);
    ~ServerEventLogger();

    bool init();
    Database::SQLiteDatabase& getServerDb();

    void logEvent(Protocol::EventType evType, const std::string& evInfo = {});

private:
    Database::SQLiteDatabase& m_serverDb;

    std::string getTimestamp() const;
};

