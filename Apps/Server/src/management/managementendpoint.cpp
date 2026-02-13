#include "managementendpoint.hpp"

#include <nlohmann/json.hpp>

#include "devicescontroller.hpp"
#include "servercontroller.hpp"
#include "devicesoftversioncontroller.hpp"

namespace Management
{

Endpoint::Endpoint(ServerEventLogger &eventLogger) :
    AbstractEndpoint(eventLogger),
    m_eventLogger {eventLogger}
{

}

Endpoint::~Endpoint()
{

}

void Endpoint::start(uint16_t port)
{
    // 1 поток оставим для этого сервера, больше незачем. Менеджер 1 за раз
    drogon::app().setThreadNum(1);

    // Настройка контроллеров
    drogon::app().registerController(std::make_shared<ServerController>());
    drogon::app().registerController(std::make_shared<DeviceSoftVersionController>(m_eventLogger));
    drogon::app().registerController(std::make_shared<DevicesController>(m_eventLogger));

    // Настройка информации по серверу
    drogon::app().setServerHeaderField("Management server");

    // TODO: Придумать что-то адекватное либо вебверсию (имеет смысл!)
    std::filesystem::create_directory("dummy");
    drogon::app().setDocumentRoot("dummy");

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
