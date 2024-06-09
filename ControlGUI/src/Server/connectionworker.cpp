#include "connectionworker.h"

Utility::Network::ConnectionWorker::ConnectionWorker(
        std::function<Exchange::Packet (const Exchange::Packet &, const QString&)>& packetProcessor,
        std::function<Exchange::Packet()>& onConnectionCallback,
        std::function<void(const QString&)>& onDisconnectedCallback,
        QObject* parent) :
    QObject(parent),
    m_pCon (new QTcpSocket(this)),
    m_processor{packetProcessor, onConnectionCallback, onDisconnectedCallback, this}
{
    connect(m_pCon, &QTcpSocket::readyRead, this, &ConnectionWorker::onMessage);
    connect(m_pCon, &QTcpSocket::disconnected, this, &ConnectionWorker::onDisconnect);
}

Utility::Network::ConnectionWorker::~ConnectionWorker()
{
    if (QTcpSocket::ConnectedState == m_pCon->state())
        m_pCon->disconnectFromHost();

    if (m_processor.onDisconnectedCallback)
    {
        m_processor.onDisconnectedCallback(m_token);
    }
}

void Utility::Network::ConnectionWorker::sendData(const QByteArray &dataString)
{
    m_pCon->write(dataString);
    m_pCon->waitForBytesWritten(1000);
    return;
}

QString Utility::Network::ConnectionWorker::getToken() const
{
    return m_token;
}

void Utility::Network::ConnectionWorker::setConnection(int descriptor, unsigned long workerId)
{
    if (QTcpSocket::ConnectedState == m_pCon->state())
    {
        qDebug() << "\033[31mUnable to set connection: Already set!\033[0m";
        return;
    }

    m_workerId = workerId;

    m_pCon->setSocketDescriptor(descriptor);

    qDebug() << "Connection created with ID: [\033[32m" << workerId << "\033[0m]";

    if (m_processor.onConnectionCallback)
    {
        auto initPacket = m_processor.onConnectionCallback();
        auto convertedPacket = Exchange::encode(initPacket);
        this->sendData(convertedPacket);
    }
}


unsigned long Utility::Network::ConnectionWorker::getId() const
{
    return m_workerId;
}


void Utility::Network::ConnectionWorker::onDisconnect()
{
    qDebug() << "Client disconnected: [\033[33m" << m_workerId << "\033[0m]";
    m_processor.onDisconnectedCallback(m_token);
    disconnect(m_pCon, &QTcpSocket::readyRead, this,&ConnectionWorker::onMessage);
    disconnect(m_pCon, &QTcpSocket::disconnected, this,&ConnectionWorker::onDisconnect);
    emit finished();
}


void Utility::Network::ConnectionWorker::onMessage()
{
    request = Exchange::decode<Exchange::Packet>(m_pCon->readAll());

    if (request.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN)
        m_token = request.payload.c_str();

    try {
    response = m_processor.process(request, m_token);
    } catch (Exchange::ConnectionException& ex)
    {
        if (ex.errorType() == Exchange::ConnectionException::ErrorType::ConnectionError)
        {
            qDebug() << "Connection error";
            m_pCon->disconnect();
            return;
        }
    }

    if (response.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_NULL_PACKET)
    {
        qDebug() << "Skipped response";
        return; // Skip packet if it have no need in response
    }
    qDebug() << "Sending request with metadata:" << response.packetMetadata;

    responsePacket = Exchange::encode(response);
    if ((m_pCon->write(responsePacket) == -1) || (!m_pCon->waitForBytesWritten()))
        qDebug() << "\033[31mError sending data to connection\033[0m";
}
