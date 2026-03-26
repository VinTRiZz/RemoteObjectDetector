#pragma once

#include <string>

#include <ROD/Types.h>

namespace DataObjects
{

/**
 * @brief The DetectorConfiguration class   Конфигурация инстанции детектора изображений
 */
struct DetectorConfiguration
{
    struct SystemInfo
    {
        id_t id {NULL_ID};
        int64_t registerDateUTC {};
    };
    SystemInfo system;

    struct OnlineInfo
    {
        int64_t lastOnlineTimeUTC   {};
        int64_t totalOnlineTime     {};
    };
    OnlineInfo online;

    std::string token;
    std::string name;
    std::string location;
};

}
