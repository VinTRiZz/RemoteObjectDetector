#include "tcpclientinstance.hpp"

#include <QTcpSocket>
#include <QThread>
#include <QCoreApplication>

#include <sys/socket.h>

namespace Utility {
namespace Network
{

const uint64_t reconnectTryCount = 100;

struct TcpCLientInstanceQ::Impl
{
    uint16_t CONNECTION_TIMEOUT {1000}, SEND_TIMEOUT {100};

    std::function<Exchange::Packet(const Exchange::Packet&)> m_requestProcessor;

    bool mustReconnect {false};
    QTcpSocket* m_client;

    QByteArray readBuf;
    Exchange::Packet m_request;
    Exchange::Packet m_response;
    long unsigned m_currentQueuePosition {0};

    QString m_hostAddress {"localhost"};
    uint16_t m_serverPort {8000};
};

TcpCLientInstanceQ::TcpCLientInstanceQ(QObject *parent) :
    QObject(parent),
    d{new Impl}
{
    qRegisterMetaType<Exchange::Packet>("Exchange::Packet");

    connect(this, &TcpCLientInstanceQ::sendMessage, this, &TcpCLientInstanceQ::sendMessageSlot);
    d->m_client = new QTcpSocket(this);
}

TcpCLientInstanceQ::~TcpCLientInstanceQ()
{
    if (isConnected())
        this->disconnect();
}

void TcpCLientInstanceQ::setupServer(const QString& address, uint16_t port)
{
    if (!isConnected())
    {
        d->m_hostAddress = address;
        d->m_serverPort = port;
    }
}

void TcpCLientInstanceQ::setPacketProcessor(std::function<Exchange::Packet (const Exchange::Packet &)> requestProcessor)
{
    d->m_requestProcessor = requestProcessor;
}

void TcpCLientInstanceQ::enableReconnectOnFail(bool reconnect)
{
    d->mustReconnect = reconnect;
}


void TcpCLientInstanceQ::setConnectionTimeout(uint16_t TIMEOUT)
{
    d->CONNECTION_TIMEOUT = TIMEOUT;
}

void TcpCLientInstanceQ::setSendTimeout(uint16_t TIMEOUT)
{
    d->SEND_TIMEOUT = TIMEOUT;
}

void TcpCLientInstanceQ::connectToServer()
{
    if (!d->m_requestProcessor)
        throw std::runtime_error("Request processor not inited. Work prohibited");

    if (isConnected()) return; // Connect only if disconnected

    d->m_client->connectToHost(d->m_hostAddress, d->m_serverPort);
    if (!d->m_client->waitForConnected(d->CONNECTION_TIMEOUT))
    {
        qDebug() << "[\033[31mE\033[0m]: CONNECTION TIMEOUT";
        return;
    }

    if (isConnected())
    {
        qDebug() << "[\033[32mS\033[0m] Connected";
        QObject::connect(d->m_client, &QTcpSocket::readyRead, this, &TcpCLientInstanceQ::onMessage);
        QObject::connect(d->m_client, &QTcpSocket::disconnected, this, &TcpCLientInstanceQ::onFail);
    }
}

void TcpCLientInstanceQ::disconnectFromServer()
{
    QObject::disconnect(d->m_client, &QTcpSocket::readyRead, this, &TcpCLientInstanceQ::onMessage);
    QObject::disconnect(d->m_client, &QTcpSocket::disconnected, this, &TcpCLientInstanceQ::onFail);

    if (isConnected()) // Disconnect only if connected
    {
        d->m_client->disconnectFromHost();
        d->m_client->waitForDisconnected(d->CONNECTION_TIMEOUT);
    }

    qDebug() << "[\033[32mS\033[0m] Disconnected manually";
}

bool TcpCLientInstanceQ::isConnected()
{
    return (d->m_client->state() == QTcpSocket::SocketState::ConnectedState);
}

void TcpCLientInstanceQ::onFail()
{
    qDebug() << "[\033[33mW\033[0m] Disconnected";

    if (d->mustReconnect)
    {
        for (int i = 0; (i < reconnectTryCount) && !isConnected(); i++)
        {
            this->disconnectFromServer();
            this->connectToServer();
            thread()->msleep(500);
        }
    }
    else
    {
        exit(1);
    }
}

void TcpCLientInstanceQ::onMessage()
{
    d->readBuf = d->m_client->readAll();
    d->m_request = Exchange::decode<Exchange::Packet>(d->readBuf);
    d->m_response = d->m_requestProcessor(d->m_request);
    if (d->m_response.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_NULL_PACKET)
        return;
    emit sendMessage(d->m_response);
}

void TcpCLientInstanceQ::sendMessageSlot(const Exchange::Packet &sendPacket)
{
    if (!isConnected()) return;

    QByteArray sendData = Exchange::encode(sendPacket);

    if (!d->m_client->isWritable())
    {
        qDebug() << "[\033[31mE\033[0m]: Is not writable";
        return;
    }

    if (d->m_client->write(sendData) == -1)
    {
        qDebug() << "[\033[31mE\033[0m]: WRITE DATA ERROR";
        return;
    }

    if (!d->m_client->waitForBytesWritten(d->SEND_TIMEOUT))
        qDebug() << "[\033[31mE\033[0m]: SEND TIMEOUT";
}

}
}

