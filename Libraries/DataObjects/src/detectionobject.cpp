#include "detectionobject.hpp"


namespace DataObjects
{

DetectionObject::DetectionObject()
{

}

DetectionObject::~DetectionObject()
{

}

void DetectionObject::setName(const std::string &nameString)
{
    m_name = nameString;
}

std::string DetectionObject::getName() const
{
    return m_name;
}

void DetectionObject::setPercent(double percent)
{
    m_percent = percent;
}

double DetectionObject::getPercent() const noexcept
{
    return m_percent;
}

bool DetectionObject::isValid() const
{
    return !m_name.empty();
}

void DetectionObject::setErrorText(const std::string &errorText)
{
    m_lastError = errorText;
}

std::string DetectionObject::getErrorText() const
{
    return m_lastError;
}

}
