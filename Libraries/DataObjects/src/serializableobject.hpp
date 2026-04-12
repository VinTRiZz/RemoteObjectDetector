#pragma once

#include <string>

namespace DataObjects
{

class SerializableObject
{
public:
    virtual std::string toJson() const = 0;
    virtual bool readJson(const std::string& iString) = 0;
};

}