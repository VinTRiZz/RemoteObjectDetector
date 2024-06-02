#include "tcpclientinstance.hpp"

#include <QTcpSocket>
#include <QThread>

namespace Utility {
namespace Network
{

struct TcpCLientInstanceQ::Impl
{
    uint16_t CONNECTION_TIMEOUT {1000}, SEND_TIMEOUT {100};

    QTcpSocket m_client;

    bool m_done {false};
    bool m_responseGot {false};

    QByteArray readBuf;
    Exchange::Packet m_response;
    long unsigned m_currentQueuePosition {0};

    QString m_hostAddress {"localhost"};
    uint16_t m_serverPort {8000};
};

TcpCLientInstanceQ::TcpCLientInstanceQ(QObject *parent) :
    QObject(parent),
    d{new Impl}
{
    QObject::connect(this, &TcpCLientInstanceQ::sendMessage, this, &TcpCLientInstanceQ::sendMessageSlot);
    qRegisterMetaType<Exchange::Packet>("Exchange::Packet");

    QObject::connect(&d->m_client, &QTcpSocket::readyRead, this, &TcpCLientInstanceQ::onMessage);
    QObject::connect(&d->m_client, &QTcpSocket::disconnected, this, &TcpCLientInstanceQ::onFail);
}

TcpCLientInstanceQ::~TcpCLientInstanceQ()
{
    QObject::disconnect(&d->m_client, &QTcpSocket::readyRead, this, &TcpCLientInstanceQ::onMessage);
    QObject::disconnect(&d->m_client, &QTcpSocket::disconnected, this, &TcpCLientInstanceQ::onFail);

    if (isConnected())
        TcpCLientInstanceQ::disconnect();
}

void TcpCLientInstanceQ::setupServer(const QString& address, uint16_t port)
{
    if (!isConnected())
    {
        d->m_hostAddress = address;
        d->m_serverPort = port;
    }
}

bool TcpCLientInstanceQ::waitForSend(int TIMEOUT)
{
    return d->m_client.waitForBytesWritten(TIMEOUT);
}

void TcpCLientInstanceQ::setConnectionTimeout(uint16_t TIMEOUT)
{
    d->CONNECTION_TIMEOUT = TIMEOUT;
}

void TcpCLientInstanceQ::setSendTimeout(uint16_t TIMEOUT)
{
    d->SEND_TIMEOUT = TIMEOUT;
}

void TcpCLientInstanceQ::connect()
{
    if (isConnected()) return; // Connect only if disconnected

    d->m_client.connectToHost(d->m_hostAddress, d->m_serverPort);

    if (!d->m_client.waitForConnected(d->CONNECTION_TIMEOUT))
    {
        qDebug() << "[\033[31mE\033[0m]: CONNECTION TIMEOUT";
        return;
    }

    d->m_done = !isConnected();

    if (!d->m_done)
    {
        if (!d->m_done) qDebug() << "[\033[32mS\033[0m] Connected";
        emit connected();
    }
}

void TcpCLientInstanceQ::disconnect()
{
    if (!isConnected()) return; // Disconnect only if connected

    d->m_done = true;
    d->m_client.disconnectFromHost();

    if (isConnected())
        d->m_client.waitForDisconnected(d->CONNECTION_TIMEOUT);

    qDebug() << "[\033[32mS\033[0m] Disconnected manually";

    if (!isConnected())
        emit disconnected();
}

bool TcpCLientInstanceQ::isConnected()
{
    return (d->m_client.state() == QTcpSocket::SocketState::ConnectedState);
}

void TcpCLientInstanceQ::sendMessageSlot(const Exchange::Packet& sendPacket)
{
    if (!isConnected()) return;

    if (sendPacket.command.empty())
    {
        qDebug() << "[\033[31mE\033[0m]: EMPTY COMMAND";
        return;
    }

    QByteArray sendData = Exchange::PacketConverter::convert(sendPacket);

    d->m_responseGot = false;
    if (d->m_client.write(sendData) == -1)
    {
        qDebug() << "[\033[31mE\033[0m]: WRITE DATA ERROR";
        return;
    }

    if (!d->m_client.waitForBytesWritten(d->SEND_TIMEOUT))
        qDebug() << "[\033[31mE\033[0m]: SEND TIMEOUT";}

Exchange::Packet TcpCLientInstanceQ::getMessage()
{
    return getMessage();
}

bool TcpCLientInstanceQ::messagesAvailable() const
{
    return d->m_responseGot;
}

void TcpCLientInstanceQ::onFail() // QAbstractSocket::SocketError errCode
{
    qDebug() << "[\033[33mW\033[0m] Disconnected";
    emit disconnected();
}

void TcpCLientInstanceQ::onMessage()
{
    d->readBuf = d->m_client.readAll();
    if (d->readBuf.size() < 2) return;
    d->m_response = Exchange::PacketConverter::convert(d->readBuf);
    d->m_responseGot = true;
    emit gotPacket(d->m_response);
}

}
}

