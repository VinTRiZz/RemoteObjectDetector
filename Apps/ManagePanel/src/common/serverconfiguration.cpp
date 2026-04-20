#include "serverconfiguration.hpp"


void ServerConfiguration::setName(const QString &name)
{
    m_name = name;
}

QString ServerConfiguration::getName() const
{
    return m_name;
}

void ServerConfiguration::setAddress(const QString &name)
{
    m_address = name;
}

QString ServerConfiguration::getAddress() const
{
    return m_address;
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
        m_address == sconf.m_address;
}