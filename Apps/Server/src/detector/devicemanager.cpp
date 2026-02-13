#include "devicemanager.hpp"

DeviceManager::DeviceManager(Database::SQLiteDatabase &db) :
    m_serverDatabase {db}
{

}

void DeviceManager::init()
{
    // TODO: Setup
}

std::optional<DeviceInfo> DeviceManager::getDevice(const std::string &id)
{
    return std::nullopt;
}
