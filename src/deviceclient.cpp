#include "deviceclient.hpp"

#include <QDebug>
#include <QThread>

#include "statusfunctions.hpp"

DeviceClient::DeviceClient(const std::string &hostAddress, uint64_t port)
{
    m_client.setupServer(QString::fromStdString(hostAddress), port);
    m_client.setPacketProcessor([this](const Exchange::Packet& request){ return processRequest(request); });
    m_client.enableReconnectOnFail(false);
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
        print("Photo in progress");
    }

    switch (request.packetMetadata)
    {
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO:
        if (request.payload != "success")
            errorGot(QString("Photo shot error: %1").arg(request.payload.c_str()));
        print("Photo shot");
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN:
        // TODO: Start download, setup temporary buffers
        print("Photo begin");
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_ADD_OBJECT:
        if (request.payload[0] != 'S')
        {
            errorGot(QString("Object add error: %1").arg(request.payload.c_str()));
            break;
        }
        print("Add object");
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
            print(QString("Object rename: %1 ---> %2").arg(prevName.c_str(), newName.c_str()));
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_REM_OBJECT:
        if (request.payload[0] != 'S')
        {
             errorGot(QString("Object remove error: %1").arg(request.payload.c_str()));
            break;
        }
        print(QString("Removing object %1").arg(std::string(request.payload.begin() + 1, request.payload.end()).c_str()));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS:
        {
            std::string token = "891duk3qwhauknhbcvulwacbiwe";

            Exchange::StatusData dev;
            dev.statusMap["CPU load"] = std::to_string(cpuLoad()) + " %";
            dev.statusMap["CPU temp."] = "65 C";
            dev.statusMap["Analyse interval"] = "1 s";
            dev.statusMap["Image send interval"] = "5 s";
            dev.statusMap["Template count"] = "30";
            dev.statusMap["Power on time"] = "02.02.2024";
            dev.statusMap["Position"] = "Somewhere in Moscow";

            Exchange::Packet devPacket(Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS, Exchange::encode(dev).toStdString());
            m_client.sendMessage(devPacket);
        }
        print("Status sent");
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP: // TODO: Setup
        print("Setup complete");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_START: // TODO: Start
        print("Started");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_START, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STOP:
        print("Stopped");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_STOP, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN:
        {
            print("Token thrown");
            Exchange::Packet tokenPacket;
            tokenPacket.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_GET_TOKEN;
            tokenPacket.payload += "qkucbikqb2t3K1237916ncasDLKHl";
            return tokenPacket;
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SET_TOKEN:
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_SET_TOKEN, "success");


    default:
         errorGot("Invalid packet metadata");
    }

    return {};
}

void DeviceClient::print(const QString &messageText)
{
    qDebug() << "Client message:" << messageText.toUtf8().data();
}

void DeviceClient::errorGot(const QString &errorText)
{
    qDebug() << "ERROR:" << errorText.toUtf8().data();
}
