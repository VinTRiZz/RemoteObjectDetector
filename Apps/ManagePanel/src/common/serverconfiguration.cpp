#include "serverconfiguration.hpp"


void ServerConfiguration::setName(const QString &name)
{
    m_name = name;
}

QString ServerConfiguration::getName() const
{
    return m_name;
}

void ServerConfiguration::setHost(const QString &hostname)
{
    m_host = hostname;
}

QString ServerConfiguration::getHost() const
{
    return m_host;
}

void ServerConfiguration::setPort(uint16_t port)
{
    m_port = port;
}

uint16_t ServerConfiguration::getPort() const
{
    return m_port;
}

bool ServerConfiguration::operator <(const ServerConfiguration& sconf) const {
    return m_name < sconf.m_name;
}

bool ServerConfiguration::operator ==(const ServerConfiguration &sconf) const
{
    return
        m_name == sconf.m_name &&
        m_port == sconf.m_port &&
        m_host == sconf.m_host;
}