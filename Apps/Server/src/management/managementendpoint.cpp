#include "managementendpoint.hpp"

#include <nlohmann/json.hpp>

#include "devicescontroller.hpp"
#include "servercontroller.hpp"
#include "devicesoftversioncontroller.hpp"

namespace Management
{

Endpoint::Endpoint() :
    AbstractEndpoint()
{

}

Endpoint::~Endpoint()
{

}

void Endpoint::start(uint16_t port)
{
    // 1 thread for status, 1 for management panel requests
    drogon::app().setThreadNum(2);

    auto pDetectorCommandProcessor = std::dynamic_pointer_cast<DetectorCommandProcessor>(m_pEventProcessor);

    // Настройка контроллеров
    drogon::app().registerController(std::make_shared<ServerController>());
    drogon::app().registerController(std::make_shared<DeviceSoftVersionController>(pDetectorCommandProcessor));
    drogon::app().registerController(std::make_shared<DevicesController>(pDetectorCommandProcessor));

    // Server info
    drogon::app().setServerHeaderField("Management server");

    // Handle Drogon needs in document root path
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
