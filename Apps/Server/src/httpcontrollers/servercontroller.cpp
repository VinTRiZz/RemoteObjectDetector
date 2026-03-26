#include "servercontroller.hpp"

#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>
#include <ROD/DeviceStatus.h>

#include <nlohmann/json.hpp>

#include "eventprocessors/servereventprocessor.hpp"

void ServerController::setServerEventProcessor(const std::shared_ptr<ServerEventProcessor> &pProcessor)
{
    m_serverEventProcessor = pProcessor;
}

void ServerController::processGetStatus(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback)
{
    // Get status
    DataObjects::DeviceStatus status;

    status.common.uptime = m_statusManager.getUptimeSec();

    status.cpu.temperature = m_statusManager.getCPUCurrentTemperature();
    status.cpu.loadPercent = m_statusManager.getCPULoad();

    auto spaceInfo = std::filesystem::space(std::filesystem::current_path());
    status.storage.spaceTotal       = spaceInfo.capacity;
    status.storage.spaceAvailable   = spaceInfo.available;
    status.storage.spaceFree        = spaceInfo.free;

    // Response
    sendJsonMessage(drogon::k200OK, status.toJson(), std::move(callback));
}

void ServerController::processPowerRequest(const drogon::HttpRequestPtr &req, ResponseCallback_t &&callback, const std::string &action)
{
    if (action == Protocol::API::PARAMATER_VALUES::POWER_OFF) {
        sendTextMessage(drogon::k200OK, "Power off started", std::move(callback));
        m_serverEventProcessor->startPoweroff();
        return;

    } else if (action == Protocol::API::PARAMATER_VALUES::POWER_REBOOT) {
        sendTextMessage(drogon::k200OK, "Reboot started", std::move(callback));
        m_serverEventProcessor->startReboot();
        return;
    }

    sendTextMessage(drogon::k400BadRequest, "Invalid action type", std::move(callback));
}
