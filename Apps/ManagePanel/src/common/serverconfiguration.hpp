#pragma once

#include <QString>
#include <QObject>

/**
 * @brief The ServerConfiguration class Object to handle servers easier
 */
class ServerConfiguration
{
public:
    // Calculated according to name, port and IP
    uint getHash() const;

    void setName(const QString& name);
    QString getName() const;

    void setAddress(const QString& name, uint16_t port);
    std::pair<QString, uint16_t> getAddress() const;

    bool operator <(const ServerConfiguration& sconf) const;

private:
    void updateHash();
    uint     m_hashCache {};
    QString  m_name;
    QString  m_address;
    uint16_t m_port {};
};
