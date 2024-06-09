#include "controlserver.h"

#include "Server/exchangepacket.h"

#include <QThread>

#include <algorithm>

ControlServer::ControlServer(QObject *parent):
    QObject{parent},
    m_server{new Utility::Network::TcpServerInstanceQ(this)}
{
    m_server->setPacketProcessor( [this](const Exchange::Packet& input, const QString& token) -> Exchange::Packet { return processPacket(input, token); } );
    m_server->setConnectionCallbacks([this](){ return onConnected(); }, [this](const QString& token){ onDisconnected(token); }  );
    qRegisterMetaType<Exchange::StatusData>("Exchange::StatusData");
}

ControlServer::~ControlServer()
{

}

bool ControlServer::init(const uint16_t portNo)
{
    return m_server->start(QHostAddress::LocalHost, portNo);
}

void ControlServer::request(Exchange::PacketMetaInfo commandCode, const QString &token, const QString payload)
{
    qDebug() << "Requesting for" << commandCode;
    Exchange::Packet requestPacket;
    requestPacket.packetMetadata = commandCode;
    requestPacket.payload = payload.toStdString();
    m_server->sendData(token, Exchange::encode(requestPacket));
}

Exchange::Packet ControlServer::processPacket(const Exchange::Packet &request, const QString& token)
{
    if (request.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_NULL_PACKET)
    {
        emit errorGot("Error packet received");
        return {};
    }

    if (request.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS)
    {
        // TODO: Use downloaded data got
        qDebug() << "Downloading photo";
    }

    switch (request.packetMetadata)
    {
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO:
        if (request.payload != "success")
            emit errorGot(QString("Photo shot error: %1").arg(request.payload.c_str()));
        qDebug() << "Object photo get succeed";
        {
            Exchange::Packet response;
            response.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN;
            return response;
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN:
        // TODO: Start download, setup temporary buffers
        qDebug() << "Started photo download";
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_ADD_OBJECT:
        if (request.payload[0] != 'S')
        {
            emit errorGot(QString("Object add error: %1").arg(request.payload.c_str()));
            break;
        }
        emit objectAdded(std::string(request.payload.begin() + 1, request.payload.end()).c_str());
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_UPD_OBJECT:
        if (!request.payload.size())
        {
            emit errorGot("Invalid response to rename object");
            break;
        }

        if (request.payload[0] != 'S')
        {
            emit errorGot(QString("Object rename error: %1").arg(request.payload.c_str()));
            break;
        }

        {
            const std::string delimeter = "]]*---*[[";
            auto delimeterPos = std::search(request.payload.begin(), request.payload.end(), delimeter.begin(), delimeter.end());
            if (delimeterPos == request.payload.end())
            {
                emit errorGot("Invalid response to add object");
                break;
            }
            std::string prevName(request.payload.begin() + 1, delimeterPos);
            std::string newName(delimeterPos + delimeter.length(), request.payload.end());
            emit objectRenamed(prevName.c_str(), newName.c_str());
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_REM_OBJECT:
        if (request.payload[0] != 'S')
        {
            emit errorGot(QString("Object remove error: %1").arg(request.payload.c_str()));
            break;
        }
        emit objectRemoved(std::string(request.payload.begin() + 1, request.payload.end()).c_str());
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS:
        qDebug() << "Status got";
        emit deviceStatusGot(Exchange::decode<Exchange::StatusData>(request.payload));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP:
        if (request.payload != "success")
            emit errorGot(QString("Setup error: %1").arg(request.payload.c_str()));
        emit deviceSetupComplete(token);
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_START:
        if (request.payload != "success")
            emit errorGot(QString("Start error: %1").arg(request.payload.c_str()));
        emit deviceStarted(token);
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STOP:
        if (request.payload != "success")
            emit errorGot(QString("Stop error: %1").arg(request.payload.c_str()));
        emit deviceStopped(token);
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN:
        if (request.payload == "ERROR")
        {
            emit errorGot("Token exchange error");
            throw Exchange::ConnectionException(Exchange::ConnectionException::ErrorType::ConnectionError);
            break;
        }
        emit deviceConnected(std::string(request.payload.begin(), request.payload.end()).c_str());
        break;


        // TODO: Write mechanism
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SET_TOKEN:
        if (request.payload == "success")
            break;
        emit errorGot(QString("Token set error: %1").arg(request.payload.c_str()));
        break;


    default:
        emit errorGot("Invalid packet metadata");
    }

    return {};
}

Exchange::Packet ControlServer::onConnected()
{
    Exchange::Packet tokenGetPacket;
    tokenGetPacket.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN;
    tokenGetPacket.payload = "";
    qDebug() << "Created init packet with metadata:" << tokenGetPacket.packetMetadata;
    return tokenGetPacket;
}

void ControlServer::onDisconnected(const QString& token)
{
    emit deviceDisconnected(token);
}
