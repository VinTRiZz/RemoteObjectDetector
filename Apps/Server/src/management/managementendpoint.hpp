#pragma once

#include <drogon/drogon.h>

#include "../endpoint/abstractendpoint.hpp"


#include <Components/SystemProcessing/StatusManager.h>

namespace Management
{

/**
 * @brief The Endpoint class Инстанция сервера, взаимодействующая с панелью управления
 */
class Endpoint : public AbstractEndpoint
{
public:
    Endpoint(Protocol::EventProcessor& serverEventProcessor, Protocol::EventProcessor &commandEventProcessor);
    ~Endpoint();

    // AbstractEndpoint interface
    void start(uint16_t port) override;
    bool isWorking() const override;
    void stop() override;

private:
    SystemProcessing::StatusManager m_systemStatusManager;
    Protocol::EventProcessor&       m_commandProcessor;
};

}
