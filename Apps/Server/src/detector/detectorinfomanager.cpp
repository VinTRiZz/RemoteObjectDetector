#include "detectorinfomanager.hpp"

#include <drogon/drogon.h>

DetectorInfoManager::DetectorInfoManager()
{

}

DetectorInfoManager::~DetectorInfoManager()
{

}

std::optional<DataObjects::DetectorConfiguration> DetectorInfoManager::getDevice(const std::string &id)
{
    return std::nullopt;
}
