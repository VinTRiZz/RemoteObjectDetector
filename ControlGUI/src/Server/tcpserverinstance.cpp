#include "tcpserverinstance.h"

#include "threadmanager.h"

namespace Utility {
namespace Network
{

#ifdef QT_NETWORK_LIB
struct TcpServerInstanceQ::Impl
{
    ThreadManager& m_conManager;

    // For packet processing
    std::function<Exchange::Packet (const Exchange::Packet &)> m_packetProcessor;
    std::function<Exchange::Packet()> m_onConnectionCallback;
    std::function<void(const QString&)> m_onDisconnectedCallback;

    Impl() : m_conManager{ThreadManager::getInstance(m_packetProcessor)}
    {

    }
};

TcpServerInstanceQ::TcpServerInstanceQ(QObject* parent):
    QTcpServer(parent),
    d {new Impl()}
{

}

TcpServerInstanceQ::~TcpServerInstanceQ()
{

}

bool TcpServerInstanceQ::start(const QHostAddress hostAddress, const unsigned port)
{
    if (!listen(hostAddress, port))
    {
        qDebug() << "\033[31mFailed to start server with text:\033[0m";
        qDebug() << errorString();
        return false;
    }

    qDebug() << "Server started" << "Host is" << hostAddress << "Port is" << port;

    return true;
}

void TcpServerInstanceQ::setConnectionCallbacks(std::function<Exchange::Packet ()> onConnectionCallback, std::function<void (const QString &)> onDisconnectedCallback)
{
    d->m_onConnectionCallback = onConnectionCallback;
    d->m_onDisconnectedCallback = onDisconnectedCallback;
    d->m_conManager.setConnectionCallbacks(d->m_onConnectionCallback, d->m_onDisconnectedCallback);
}

void TcpServerInstanceQ::setPacketProcessor(std::function<Exchange::Packet (const Exchange::Packet &)> packetProcessor)
{
    d->m_packetProcessor = packetProcessor;
}

bool TcpServerInstanceQ::sendData(const QString &connectionToken, const QByteArray &data)
{
    return d->m_conManager.sendData(connectionToken, data);
}

void TcpServerInstanceQ::incomingConnection(qintptr handle)
{
    d->m_conManager.createConnection(handle);
}
#endif // QT_NETWORK_LIB






TcpServerInstance::TcpServerInstance()
{

}

TcpServerInstance::~TcpServerInstance()
{

}


}
}
