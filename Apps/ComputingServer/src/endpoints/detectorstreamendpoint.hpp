#pragma once

#include <ROD/Servers/AbstractEndpoint.h>

#include <Components/Network/ServerUDP.h>
#include <Components/Network/ClientUDP.h>

#include <ROD/ImageProcessing/ImageProcessor.h>
#include <ROD/Exchange/SendableImage.h>

#include <map>
#include <set>

/**
 * @brief The DetectorStreamEndpoint class  Server instance, processing video streaming and retranslation
 */
class DetectorStreamEndpoint : public AbstractEndpoint
{
public:
    DetectorStreamEndpoint();
    ~DetectorStreamEndpoint();

    // AbstractEndpoint interface
    void start(uint16_t port) override;
    bool isWorking() const override;
    void stop() override;

    /**
     * @brief setImageReceivedCallback When image received, this callback will be called once in some thread
     * @param imgCallback
     */
    void setImageReceivedCallback(std::function<void(Protocol::SendableImage&&)>&& imgCallback);

    /**
     * @brief The ImageInfo class Detector sent image parts
     */
    struct ImageInfo
    {
        std::mutex addMx;
        uint64_t id {};
        std::set<Protocol::ImagePacket> parts;
    };

private:
    UDP::Server m_streamingServer;      // Receiver inserting images into processor to proceed
    UDP::Client m_streamingDataSender;  // Retranslator

    std::mutex m_imageMx;
    std::map<uint64_t, ImageInfo>                   m_imageParts;
    std::function<void(Protocol::SendableImage&&)>  m_receivedCallback;
};

