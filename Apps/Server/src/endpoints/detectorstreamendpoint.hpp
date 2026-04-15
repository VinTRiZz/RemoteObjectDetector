#pragma once

#include "abstractendpoint.hpp"

#include <Components/Network/ServerUDP.h>
#include <Components/Network/ClientUDP.h>

#include <ROD/ImageProcessing/ImageProcessor.h>

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

private:
    UDP::Server m_streamingServer;      // Receiver inserting images into processor to proceed
    UDP::Client m_streamingDataSender;  // Retranslator

    ImageProcessing::Processor imgProcessor {std::thread::hardware_concurrency()};
};

