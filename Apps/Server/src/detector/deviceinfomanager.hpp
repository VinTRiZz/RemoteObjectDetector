#pragma once

#include <string>

#include <Components/Database/SQlite.h>

#include <ROD/DetectorConfiguration.h>

class DeviceInfoManager
{
public:
    explicit DeviceInfoManager(Database::SQLiteDatabase& db);

    void init();

    std::optional<DataObjects::DetectorConfiguration> getDevice(const std::string& id);

private:
    Database::SQLiteDatabase &m_serverDatabase;
    std::shared_ptr<Database::SQLiteTable> m_devicesTable;
};

