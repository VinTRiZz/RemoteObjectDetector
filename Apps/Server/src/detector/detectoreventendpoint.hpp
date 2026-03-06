#pragma once

#include "../endpoint/abstractendpoint.hpp"

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <set>

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
    using Server = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHdl = websocketpp::connection_hdl;
    using MessagePtr = websocketpp::config::asio::message_type::ptr;

    websocketpp::lib::asio::io_service m_ioService;
    std::unique_ptr<std::thread> m_ioThread;

    Server m_deviceEventServer;
    std::set<ConnectionHdl, std::owner_less<ConnectionHdl>> m_connections;
    mutable std::mutex m_connectionMx;

    std::atomic<bool> m_isListening {false};
};
