#pragma once

#include <string>

#include <Components/Database/SQlite.h>

#include <ROD/DetectorConfiguration.h>

class DetectorInfoManager
{
public:
    explicit DetectorInfoManager();
    ~DetectorInfoManager();

    std::optional<DataObjects::DetectorConfiguration> getDevice(const std::string& id);

private:
};

