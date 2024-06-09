#include "controlserver.h"

#include "Server/exchangepacket.h"

#include <QThread>
#include <QCoreApplication>

#include <algorithm>
#include <nlohmann/json.hpp>

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

bool ControlServer::isConnected(const QString &token)
{
    return m_server->hasConnection(token);
}

void ControlServer::setImageSize(uint64_t height, uint64_t width)
{
    m_maxImageHeight = height;
    m_maxImageWidth = width;
}

void ControlServer::photo(const QString &token)
{
    nlohmann::json requestSizes;
    requestSizes["maxH"] = m_maxImageHeight;
    requestSizes["maxW"] = m_maxImageWidth;
    request(Exchange::PACKET_INFO_CT_PHOTO, token, requestSizes.dump().c_str());
}

void ControlServer::renameObject(const QString &objectName, const QString &newName, const QString &token)
{
    nlohmann::json requestRenames;
    requestRenames["oldName"] = objectName.toUtf8().data();
    requestRenames["newName"] = newName.toUtf8().data();
    request(Exchange::PACKET_INFO_CT_REM_OBJECT, token, requestRenames.dump().c_str());
}

QImage ControlServer::getPhoto()
{
    if (!m_imageDataBuffer.use_count())
        return {};

    // Convert the cv::Mat to a QImage
    return QImage(reinterpret_cast<uchar*>(m_imageDataBuffer.get()), m_imageCols, m_imageRows, QImage::Format_RGB888);
}

void ControlServer::request(Exchange::PacketMetaInfo commandCode, const QString &token, const QString payload)
{
    Exchange::Packet requestPacket;
    requestPacket.packetMetadata = commandCode;
    requestPacket.payload = payload.toStdString();
    m_server->sendData(token, Exchange::encode(requestPacket));
    QCoreApplication::processEvents();
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
        if (!request.payload.size())
            return {};
        std::copy(request.payload.begin(), request.payload.end(), m_imageDataBuffer.get() + m_currentPos);
        m_currentPos += request.payload.size();
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS, std::to_string(m_currentPos));
    }

    switch (request.packetMetadata)
    {
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO:
        if (request.payload != "success")
            emit errorGot(QString("Photo shot error: %1").arg(request.payload.c_str()));
        m_currentPos = 0;
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN, "");

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_END:
        emit photoGot();
        break;

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN:
        {
            nlohmann::json objectsPacketJson;
            try {
                objectsPacketJson = nlohmann::json::parse(request.payload);
            } catch (nlohmann::json::exception& ex)
            {
                emit errorGot("Error parsing detected object list");
                return {};
            }
            m_imageCols = objectsPacketJson["cols"];
            m_imageRows = objectsPacketJson["rows"];
            m_photoSize = objectsPacketJson["size"];
            m_imageDataBuffer = std::shared_ptr<char>(new char[m_photoSize]);
        }
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS, "0");

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_LIST:
        qDebug() << "Got object list : " << request.payload.c_str();
        emit objectListGot(token, request.payload.c_str());
        break;

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_DETECTED:
        qDebug() << "Got detected list : " << request.payload.c_str();
        emit objectDetectedListGot(token, request.payload.c_str());
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
        emit deviceStatusGot(Exchange::decode<Exchange::StatusData>(request.payload));
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP:
        if (request.payload != "success")
            emit errorGot(QString("Setup error: %1").arg(request.payload.c_str()));
        emit deviceSetupComplete(token);
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_LIST, "");


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
    return tokenGetPacket;
}

void ControlServer::onDisconnected(const QString& token)
{
    emit deviceDisconnected(token);
}
