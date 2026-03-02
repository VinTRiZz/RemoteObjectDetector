#pragma once

#include <string>

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

    void setName(const std::string& nameString);
    std::string getName() const;

    void setPercent(double percent);
    double getPercent() const noexcept;

    bool isValid() const;

    void setErrorText(const std::string& errorText);
    std::string getErrorText() const;

private:
    std::string m_name;
    double m_percent {1};

    std::string m_lastError;
};

}
