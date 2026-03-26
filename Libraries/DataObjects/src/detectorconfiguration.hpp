#pragma once

#include <string>

#include <ROD/Types.h>
#include <ROD/Error.h>
#include "serializableobject.hpp"

namespace DataObjects
{

/**
 * @brief The DetectorConfiguration class   Конфигурация инстанции детектора изображений
 */
class DetectorConfiguration: public SerializableObject,
                             public ErrorUser
{
public:
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

    struct Security
    {
        std::string token {};
    };
    Security security;

    struct Info
    {
        std::string name;
        std::string location;
    };
    Info info;

    // SerializableObject interface
    std::string toJson() override;
    bool readJson(const std::string &iString) override;
};

}
