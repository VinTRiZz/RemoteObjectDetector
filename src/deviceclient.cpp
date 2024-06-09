#include "deviceclient.hpp"

#include <QDebug>
#include <QThread>
#include <nlohmann/json.hpp>

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
        uint64_t currentPos = 0;
        try {
            currentPos = std::stoul(request.payload);
        } catch (std::invalid_argument& ex)
        {
            print(QString("Error converting payload \"%1\" to pos").arg(request.payload.c_str()));
            return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS, "");
        }

        if (currentPos >= m_imageBuffer.dataend - m_imageBuffer.datastart) return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_END, "");

        Exchange::Packet response(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_IN_PROCESS, "");

        if ((m_imageBuffer.dataend - m_imageBuffer.datastart - currentPos) >= Exchange::DOWNLOAD_PAYLOAD_SIZE)
        {
            response.payload.resize(Exchange::DOWNLOAD_PAYLOAD_SIZE);
            std::copy(m_imageBuffer.datastart + currentPos, m_imageBuffer.datastart + currentPos + Exchange::DOWNLOAD_PAYLOAD_SIZE, response.payload.begin());
        }
        else
        {
            response.payload.resize(m_imageBuffer.dataend - m_imageBuffer.datastart - currentPos);
            std::copy(m_imageBuffer.datastart + currentPos, m_imageBuffer.dataend, response.payload.begin());
        }
        return response;
    }

    switch (request.packetMetadata)
    {
    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO:
        m_imageBuffer = m_analyseInterface.getCameraShot();
        if (m_imageBuffer.empty())
            return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO, "Image shot is empty");

        cv::cvtColor(m_imageBuffer, m_imageBuffer, cv::COLOR_BGR2RGB);

        try {
            auto imageSizesJson = nlohmann::json::parse(request.payload);
            uint64_t imageMaxHeight = imageSizesJson["maxH"];
            uint64_t imageMaxWidth = imageSizesJson["maxW"];
            float scaleCoeff = (float)m_imageBuffer.cols / (float)m_imageBuffer.rows;
            cv::Size newSize(imageMaxHeight * scaleCoeff, imageMaxWidth / scaleCoeff);
            cv::resize(m_imageBuffer, m_imageBuffer, newSize);
        } catch (nlohmann::json::exception& ex)
        {
            print("JSON parsing error");
            return {};
        }

        print("Photo shot");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN:
        {
            nlohmann::json imageProperties;
            imageProperties["cols"] = m_imageBuffer.cols;
            imageProperties["rows"] = m_imageBuffer.rows;
            imageProperties["size"] = m_imageBuffer.dataend - m_imageBuffer.datastart;
            print("Photo begin");
            return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_PHOTO_BEGIN, imageProperties.dump());
        }

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_LIST:
        {
            Exchange::Packet response;
            response.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_LIST;
            auto objects = m_analyseInterface.availableTypes();
            for (auto& obj : objects)
            {
                response.payload += obj.name + ";";
            }
            print(QString("Object list asked. List: %1").arg(response.payload.c_str()));
            return response;
        }
        break;

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_DETECTED:
        {
            Exchange::Packet response;
            response.packetMetadata = Exchange::PacketMetaInfo::PACKET_INFO_CT_DETECTED;
            auto objects = m_analyseInterface.detectedObjects();
            nlohmann::json objectsPacketJson;
            for (auto& obj : objects)
            {
                objectsPacketJson[obj.name] = std::to_string(obj.percent);
            }
            response.payload = objectsPacketJson.dump();
            print("Detected asked");
            return response;
        }
        break;

    case Exchange::PacketMetaInfo::PACKET_INFO_CT_REBOOT:
#warning "Reboot disabled"
        // system("reboot");
        print("Reboot called");
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_ADD_OBJECT:
        if (request.payload[0] != 'S')
        {
            errorGot(QString("Object add error: %1").arg(request.payload.c_str()));
            break;
        }
        {
            ObjectType _ot;
            _ot.name = request.payload;
            _ot.imagePath = std::string("addedTemplates/") + _ot.name + ".jpg"; // TODO: Use this in program
            m_analyseInterface.addType(_ot);
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
            print(QString("Object rename called: %1 ---> %2").arg(prevName.c_str(), newName.c_str()));
            return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_UPD_OBJECT, m_analyseInterface.renameType(prevName, newName) ? "success" : m_analyseInterface.lastErrorText());
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_REM_OBJECT:
        if (request.payload[0] != 'S')
        {
             errorGot(QString("Object remove error: %1").arg(request.payload.c_str()));
            break;
        }
        {
            auto objectName = std::string(request.payload.begin() + 1, request.payload.end());
            print(QString("Removing object %1").arg(objectName.c_str()));
            m_analyseInterface.removeType(objectName);
            return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_REM_OBJECT,  "success");
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS:
        {
            std::string token = "891duk3qwhauknhbcvulwacbiwe";

            std::string cpuLoadStr = std::to_string(cpuLoad());
            cpuLoadStr.erase(cpuLoadStr.begin() + 5, cpuLoadStr.end());

            std::string cpuTempStr = std::to_string(cpuTemperature());
            cpuTempStr.erase(cpuTempStr.begin() + 5, cpuTempStr.end());

            Exchange::StatusData dev;
            dev.statusMap["CPU load"] = cpuLoadStr + " %";
            dev.statusMap["CPU temp."] = cpuTempStr + " C";
            dev.statusMap["Template count"] = std::to_string(m_analyseInterface.availableTypesCount());
            dev.statusMap["Init succeed"] = m_analyseInterface.isReady() ? "true" : "false";
            dev.statusMap["Init last error text"] = m_analyseInterface.lastErrorText();
            dev.statusMap["Power on time"] = getStartTime();

            Exchange::Packet devPacket(Exchange::PacketMetaInfo::PACKET_INFO_CT_STATUS, Exchange::encode(dev).toStdString());
            m_client.sendMessage(devPacket);
        }
        break;


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP:
        print("Loading template images");
        m_analyseInterface.setCamera("/dev/camera0");
        m_analyseInterface.init();
        m_analyseInterface.processTemplatesDirectory("templates");
        print("Setup complete");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_SETUP, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_START:
        m_analyseInterface.startDetectObjects();
        print("Started");
        return Exchange::Packet(Exchange::PacketMetaInfo::PACKET_INFO_CT_START, "success");


    case Exchange::PacketMetaInfo::PACKET_INFO_CT_STOP: // TODO: Interrupt
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
