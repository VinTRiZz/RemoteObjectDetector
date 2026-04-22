#include "managementendpoint.hpp"

#include <nlohmann/json.hpp>

#include "httpcontrollers/servercontroller.hpp"

namespace Management
{

Endpoint::Endpoint() :
    AbstractEndpoint()
{

}

Endpoint::~Endpoint()
{

}

void Endpoint::setRecordManager(const Database::RecordManagerPtr &pManager)
{
    m_pRecordManager = pManager;
}

void Endpoint::start(uint16_t port)
{
    // 1 thread for status, 1 for management panel requests
    drogon::app().setThreadNum(2);

    // Настройка контроллеров
    drogon::app().registerController(std::make_shared<ServerController>());

    // Server info
    drogon::app().setServerHeaderField("Management server");

    drogon::app().addListener("0.0.0.0", port);
    drogon::app().run();
}

bool Endpoint::isWorking() const
{
    return drogon::app().isRunning();
}

void Endpoint::stop()
{
    if (!Endpoint::isWorking()) {
        return;
    }
    drogon::app().quit();
}

}
