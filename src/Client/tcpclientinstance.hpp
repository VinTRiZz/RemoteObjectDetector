#ifndef UL_TCPCLIENTINSTANCE_H
#define UL_TCPCLIENTINSTANCE_H

#include <memory>

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
    bool waitForSend(int TIMEOUT = 10000);

    void setConnectionTimeout(uint16_t TIMEOUT = 1000);
    void setSendTimeout(uint16_t TIMEOUT = 1000);


public slots:
    void connect();
    void disconnect();

public:
    bool isConnected();
    Exchange::Packet getMessage();
    bool messagesAvailable() const;
    QString errorText() const;

signals:
    void sendMessage(const Exchange::Packet& sendPacket);

    // Used to process packets got from server
    void gotPacket(const Exchange::Packet& p);

    void connected();
    void disconnected();

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
