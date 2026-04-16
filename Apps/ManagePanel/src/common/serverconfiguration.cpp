#include "serverconfiguration.hpp"


uint ServerConfiguration::getHash() const
{
    return m_hashCache;
}

void ServerConfiguration::setName(const QString &name)
{
    m_name = name;
    updateHash();
}

QString ServerConfiguration::getName() const
{
    return m_name;
}

void ServerConfiguration::setAddress(const QString &name, uint16_t port)
{
    m_address = name;
    m_port = port;
    updateHash();
}

std::pair<QString, uint16_t> ServerConfiguration::getAddress() const
{
    return std::make_pair(m_address, m_port);
}

bool ServerConfiguration::operator <(const ServerConfiguration& sconf) const {
    return (m_name < sconf.m_name);
}

void ServerConfiguration::updateHash()
{
    m_hashCache =   qHash(m_name) >> 2 |
                    qHash(m_address) >> 1 |
                    qHash(m_port);
}