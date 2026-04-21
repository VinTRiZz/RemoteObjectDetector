#include "detectorstreamendpoint.hpp"

#include <Components/Logger/Logger.h>

#include <thread>

DetectorStreamEndpoint::DetectorStreamEndpoint() :
    AbstractEndpoint()
{
    m_streamingServer.setRequestProcessor([this](std::vector<uint8_t>&& udpRecData){
        if (!m_receivedCallback) {
            COMPLOG_WARNING("Image packet ignored (no processor set)");
            return;
        }

        std::thread([this, receivedData = std::move(udpRecData)](){
            Protocol::ImagePacket pkt;
            if (!pkt.initFromPacketPart(receivedData)) {
                return;
            }
            auto senderId = pkt.getSenderId();

            m_imageMx.lock();
            auto& imgData = m_imageParts[senderId];
            m_imageMx.unlock();

            std::lock_guard<std::mutex> lock(imgData.addMx);
            if (imgData.id != pkt.getId()) {
                imgData.parts.clear();
            }
            imgData.id = pkt.getId();
            imgData.parts.emplace(std::move(pkt));
            Protocol::SendableImage img;
            if (!img.canInitFrom(imgData.parts)) {
                return;
            }
            img.initFromPackets(std::move(imgData.parts));
            imgData.id = 0;
            m_receivedCallback(std::move(img));
        }).detach();
    });
}

DetectorStreamEndpoint::~DetectorStreamEndpoint()
{
    DetectorStreamEndpoint::stop();
}

void DetectorStreamEndpoint::start(uint16_t port)
{
    m_streamingServer.start(port);
}

bool DetectorStreamEndpoint::isWorking() const
{
    return m_streamingServer.isWorking();
}

void DetectorStreamEndpoint::stop()
{
    m_streamingServer.stop();
}

void DetectorStreamEndpoint::setImageReceivedCallback(std::function<void (Protocol::SendableImage &&)> &&imgCallback)
{
    m_receivedCallback = std::move(imgCallback);
}
