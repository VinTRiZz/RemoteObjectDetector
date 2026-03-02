#pragma once

#include <string>

namespace DataObjects
{

/**
 * @brief The DetectorConfiguration class   Конфигурация инстанции детектора изображений
 */
struct DetectorConfiguration
{
    std::string id;
    std::string token;
    std::string name;
    std::string location;
};

}
