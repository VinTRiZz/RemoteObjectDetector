#pragma once

#include <string>

#include <Components/Database/SQlite.h>

struct DeviceInfo
{
    std::string id;
    std::string name;

    enum DeviceStatus : int
    {
        UnknownDevice,
        Offline,
        Idle,
        Working,
    };
    DeviceStatus status {DeviceStatus::UnknownDevice};
};

class DeviceManager
{
public:
    explicit DeviceManager(Database::SQLiteDatabase& db);

    void init();

    std::optional<DeviceInfo> getDevice(const std::string& id);

private:
    Database::SQLiteDatabase &m_serverDatabase;
    std::shared_ptr<Database::SQLiteTable> m_devicesTable;
};

