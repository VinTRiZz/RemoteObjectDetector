#ifndef DEVICECLIENT_HPP
#define DEVICECLIENT_HPP

#include "Client/exchangepacket.hpp"
#include "Client/tcpclientinstance.hpp"

class DeviceClient
{
public:
    DeviceClient(const std::string& hostAddress, uint64_t port);

    void connectToServer();

private:
    Utility::Network::TcpCLientInstanceQ m_client;

    Exchange::Packet processRequest(const Exchange::Packet& request);

    void errorGot(const QString& errorText);
};

#endif // DEVICECLIENT_HPP
