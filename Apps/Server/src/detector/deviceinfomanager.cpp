#include "deviceinfomanager.hpp"

DeviceInfoManager::DeviceInfoManager(Database::SQLiteDatabase &db) :
    m_serverDatabase {db}
{

}

void DeviceInfoManager::init()
{
    // TODO: Setup
}

std::optional<DataObjects::DetectorConfiguration> DeviceInfoManager::getDevice(const std::string &id)
{
    return std::nullopt;
}
