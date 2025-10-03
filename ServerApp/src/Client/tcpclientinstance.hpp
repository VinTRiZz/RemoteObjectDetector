#ifndef UL_TCPCLIENTINSTANCE_H
#define UL_TCPCLIENTINSTANCE_H

#include <memory>
#include <functional>

#include "exchangepacket.hpp"

#include <QString>
#include <QObject>

namespace Utility {
namespace Network {

class TcpCLientInstanceQ : public QObject
{
    Q_OBJECT
public:
    TcpCLientInstanceQ(QObject * parent = 0);
    ~TcpCLientInstanceQ();

    void setupServer(const QString& address, uint16_t port);
    void setPacketProcessor(std::function<Exchange::Packet(const Exchange::Packet&)> requestProcessor);

    void enableReconnectOnFail(bool reconnect = true);

    void setConnectionTimeout(uint16_t TIMEOUT = 1000);
    void setSendTimeout(uint16_t TIMEOUT = 1000);

    bool isConnected();
    QString errorText() const;

signals:
    void sendMessage(const Exchange::Packet& sendPacket);

public slots:
    void connectToServer();
    void disconnectFromServer();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

private slots:
    void onFail();
    void onMessage();
    void sendMessageSlot(const Exchange::Packet& sendPacket);
};

}

}
#endif // UL_TCPCLIENTINSTANCE_H
