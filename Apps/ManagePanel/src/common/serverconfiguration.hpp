#pragma once

#include <QString>
#include <QObject>

/**
 * @brief The ServerConfiguration class Object to handle servers easier
 */
class ServerConfiguration
{
public:
    void setName(const QString& name);
    QString getName() const;

    void setHost(const QString& hostname);
    QString getHost() const;

    void setPort(uint16_t port);
    uint16_t getPort() const;

    bool operator <(const ServerConfiguration& sconf) const;
    bool operator ==(const ServerConfiguration& sconf) const;

private:
    QString  m_name;
    QString  m_host;
    uint16_t m_port {};
};
