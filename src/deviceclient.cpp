#include "deviceclient.hpp"

#include <QDebug>
#include <QThread>

DeviceClient::DeviceClient(const std::string &hostAddress, uint64_t port)
{
    m_client.setupServer(QString::fromStdString(hostAddress), port);
    m_client.setPacketProcessor([this](const Exchange::Packet& request){ return processRequest(request); });
    m_client.enableReconnectOnFail();
}

void DeviceClient::connectToServer()
{
    while (!m_client.isConnected())
    {
        m_client.connectToServer();
        m_client.thread()->msleep(500);
    }
}

Exchange::Packet DeviceClient::processRequest(const Exchange::Packet &request)
{
    if (request.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_NULL_PACKET)
    {
        errorGot("Error packet received");
        return {};
    }

    if (request.packetMetadata == Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS)
    {
        // TODO: Use downloaded data got
    }

    switch (request.packetMetadata)
    {
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO:
        if (request.payload != "success")
            errorGot(QString("Photo shot error: %1").arg(request.payload.c_str()));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN:
        // TODO: Start download, setup temporary buffers
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_ADD_OBJECT:
        if (request.payload[0] != 'S')
        {
            errorGot(QString("Object add error: %1").arg(request.payload.c_str()));
            break;
        }
        qDebug() << "Object add request";
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_UPD_OBJECT:
        if (!request.payload.size())
        {
            errorGot("Invalid response to add object");
            break;
        }

        if (request.payload[0] != 'S')
        {
            errorGot(QString("Object rename error: %1").arg(request.payload.c_str()));
            break;
        }

        {
            const std::string delimeter = "]]*---*[[";
            auto delimeterPos = std::search(request.payload.begin(), request.payload.end(), delimeter.begin(), delimeter.end());
            if (delimeterPos == request.payload.end())
            {
                errorGot("Invalid response to add object");
                break;
            }
            std::string prevName(request.payload.begin() + 1, delimeterPos);
            std::string newName(delimeterPos + delimeter.length(), request.payload.end());
            qDebug() << "Renaming object:" << prevName.c_str() << newName.c_str();
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_REM_OBJECT:
        if (request.payload[0] != 'S')
        {
             errorGot(QString("Object remove error: %1").arg(request.payload.c_str()));
            break;
        }
        qDebug() << "Removing object:" << std::string(request.payload.begin() + 1, request.payload.end()).c_str();
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS:
        {
            std::string token = "891duk3qwhauknhbcvulwacbiwe";

            Exchange::StatusData dev;
            dev.statusMap["CPU load"] = "33 %";
            dev.statusMap["CPU temp."] = "65 C";
            dev.statusMap["Analyse interval"] = "1 s";
            dev.statusMap["Image send interval"] = "5 s";
            dev.statusMap["Template count"] = "30";
            dev.statusMap["Power on time"] = "02.02.2024";
            dev.statusMap["Position"] = "Somewhere in Moscow";

            Exchange::Packet devPacket;
            devPacket.payload = Exchange::encode(dev).toStdString();
            m_client.sendMessage(devPacket);
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP:
        if (request.payload != "success")
             errorGot(QString("Setup error: %1").arg(request.payload.c_str()));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_START:
        if (request.payload != "success")
             errorGot(QString("Start error: %1").arg(request.payload.c_str()));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STOP:
        if (request.payload != "success")
             errorGot(QString("Stop error: %1").arg(request.payload.c_str()));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN:
        {
            Exchange::Packet tokenPacket;
            tokenPacket.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN;
            tokenPacket.payload = "S"; // Mean that operation succeed
            tokenPacket.payload += "qkucbikqb2t3K1237916ncasDLKHl";
            return tokenPacket;
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SET_TOKEN:
        {
            Exchange::Packet tokenPacket;
            tokenPacket.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_SET_TOKEN;
            tokenPacket.payload = "success";
            return tokenPacket;
        }
        break;


    default:
         errorGot("Invalid packet metadata");
    }

    return {};
}

void DeviceClient::errorGot(const QString &errorText)
{
    qDebug() << "ERROR:" << errorText.toUtf8().data();
}
