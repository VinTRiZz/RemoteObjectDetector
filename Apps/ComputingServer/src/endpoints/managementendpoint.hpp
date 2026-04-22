#pragma once

#include <drogon/drogon.h>

#include <ROD/Servers/AbstractEndpoint.h>
#include <ROD/Servers/Records.h>

#include <Components/SystemProcessing/StatusManager.h>

namespace Management
{

/**
 * @brief The Endpoint class Инстанция сервера, взаимодействующая с панелью управления
 */
class Endpoint : public AbstractEndpoint
{
public:
    Endpoint();
    ~Endpoint();

    void setRecordManager(const Database::RecordManagerPtr& pManager);

    // AbstractEndpoint interface
    void start(uint16_t port) override;
    bool isWorking() const override;
    void stop() override;

private:
    Database::RecordManagerPtr m_pRecordManager;
};

}
