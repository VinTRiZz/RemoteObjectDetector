#pragma once

#include <string>

namespace DataObjects
{

class SerializableObject
{
public:
    virtual std::string toJson() = 0;
    virtual bool readJson(const std::string& iString) = 0;
};

}