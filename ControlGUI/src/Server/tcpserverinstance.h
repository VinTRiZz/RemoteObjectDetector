#ifndef UL_TCPSERVERINSTANCE_H
#define UL_TCPSERVERINSTANCE_H

#include <memory>

#ifdef QT_NETWORK_LIB
#include <QTcpServer>
#include <QHostAddress>
#endif

#include "exchangepacket.h"

namespace Utility
{

namespace Network
{

// Qt-only server
#ifdef QT_NETWORK_LIB
class TcpServerInstanceQ : public QTcpServer
{
public:
    TcpServerInstanceQ(QObject *parent = nullptr);
    ~TcpServerInstanceQ();

    bool start(const QHostAddress hostAddress, const unsigned port);

    void setConnectionCallbacks(std::function<Exchange::Packet()> onConnectionCallback, std::function<void(const QString&)> onDisconnectedCallback);
    void setPacketProcessor(std::function<Exchange::Packet (const Exchange::Packet &, const QString &)> packetProcessor);
    bool sendData(const QString& connectionToken, const QByteArray& data);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void incomingConnection(qintptr handle) override;
};
#endif // QT_NETWORK_LIB




class TcpServerInstance
{
public:
    TcpServerInstance();
    ~TcpServerInstance();   
};

}

}

#endif // UL_TCPSERVERINSTANCE_H
