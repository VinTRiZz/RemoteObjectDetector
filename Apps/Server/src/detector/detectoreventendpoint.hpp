#pragma once

#include "../endpoint/abstractendpoint.hpp"

#include <Components/Network/ServerWS.h>

/**
 * @brief The DetectorEventEndpoint class Detector event management instance
 */
class DetectorEventEndpoint : public AbstractEndpoint
{
public:
    DetectorEventEndpoint(ServerEventLogger& eventLogger);
    ~DetectorEventEndpoint();

    // AbstractEndpoint interface
    void start(uint16_t port) override;
    bool isWorking() const override;
    void stop() override;

private:
    WebSockets::Server m_deviceEventServer; // Event retranslator with a bit processing
};
