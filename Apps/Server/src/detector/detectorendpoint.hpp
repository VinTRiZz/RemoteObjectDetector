#pragma once

#include "../endpoint/abstractendpoint.hpp"

namespace Detector
{

/**
 * @brief The Endpoint class Инстанция сервера, информирующая о событиях на детекторах
 */
class Endpoint : public AbstractEndpoint
{
public:
    Endpoint(ServerEventLogger& eventLogger);
    ~Endpoint();

    // AbstractEndpoint interface
    void start(uint16_t port) override;
    bool isWorking() const override;
    void stop() override;

private:
    Database::SQLiteDatabase& m_db;
};

}
