#pragma once

#include <string>

#include <ROD/Types.h>

namespace DataObjects
{

/**
 * @brief The DetectionObject class Класс для хранения информации об обнаруживаемом объекте
 */
class DetectionObject
{
public:
    DetectionObject();
    ~DetectionObject();

    void setId(id_t id);
    id_t getId() const;

    void setName(const std::string& nameString);
    std::string getName() const;

    void setPercent(double percent);
    double getPercent() const noexcept;

    bool isValid() const;

    void setErrorText(const std::string& errorText);
    std::string getErrorText() const;

private:
    id_t m_id {NULL_ID};

    std::string m_name;
    double m_percent {1};

    std::string m_lastError;
};

}
