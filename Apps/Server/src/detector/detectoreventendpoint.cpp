#include "detectoreventendpoint.hpp"

#include <Components/Logger/Logger.h>

DetectorEventEndpoint::DetectorEventEndpoint(ServerEventLogger &eventLogger) :
    AbstractEndpoint(eventLogger)
{
    m_deviceEventServer.init_asio(&m_ioService);

    m_deviceEventServer.set_reuse_addr(true);

    m_deviceEventServer.set_open_handler([this](ConnectionHdl hdl) {
        m_connectionMx.lock();
        m_connections.insert(hdl);
        m_connectionMx.unlock();

        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        auto remote = con->get_remote_endpoint();
        COMPLOG_OK("[WS] Client connected:", remote);
    });

    m_deviceEventServer.set_close_handler([this](ConnectionHdl hdl) {
        m_connectionMx.lock();
        auto it = m_connections.find(hdl);
        if (it != m_connections.end()) {
            m_connections.erase(it);
        }
        m_connectionMx.unlock();

        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        auto code = con->get_remote_close_code();
        auto reasonStr = con->get_remote_close_reason();

        switch (code)
        {
        case websocketpp::close::status::normal:
            COMPLOG_WARNING("[WS] Client disconnected:", reasonStr, "(normal)");
            break;

        case websocketpp::close::status::going_away:
            COMPLOG_WARNING("[WS] Client disconnected:", reasonStr, "(server shutdown)");
            break;

        case websocketpp::close::status::protocol_error:
            COMPLOG_WARNING("[WS] Client disconnected:", reasonStr, "(protocol error)");
            break;

        default:
            COMPLOG_WARNING("[WS] Client disconnected:", reasonStr, "code:", code);
        };
    });

    m_deviceEventServer.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            std::string payload = msg->get_payload();
            COMPLOG_DEBUG("[WS] Text data got:", payload);
                try {
                    auto eventRequest = nlohmann::json::parse(payload);
                } catch (nlohmann::json::exception& parseEx) {

                }
            return;
        }
        COMPLOG_WARNING("Received binary data (skipped)");
    });

    m_deviceEventServer.set_fail_handler([this](ConnectionHdl hdl) {
        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        auto ec = con->get_ec();
        COMPLOG_ERROR("[WS] Connection:", ec.message());
    });

    m_deviceEventServer.set_http_handler([this](ConnectionHdl hdl) {
        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        con->set_body("Invalid protocol (required WebSockets)");
        con->set_status(websocketpp::http::status_code::not_found);
    });
}

DetectorEventEndpoint::~DetectorEventEndpoint()
{
    DetectorEventEndpoint::stop();
}

void DetectorEventEndpoint::start(uint16_t port)
{
    websocketpp::lib::asio::ip::tcp::endpoint endpoint(
        websocketpp::lib::asio::ip::make_address("0.0.0.0"),
        port
    );

    websocketpp::lib::error_code ec;
    m_deviceEventServer.listen(endpoint, ec);
    if (ec) {
        COMPLOG_ERROR("[WS] Can't start event server:", ec.message());
        return;
    }

    m_deviceEventServer.start_accept(ec);
    if (ec) {
        COMPLOG_ERROR("[WS] Start accept error:", ec.message());
        return;
    }

    if (!m_ioThread) {
        m_ioThread = std::make_unique<std::thread>([this]() {
            m_deviceEventServer.run();
        });
    }
    m_isListening = true;

    COMPLOG_OK("[WS] Started event server on port", std::to_string(port));

}

bool DetectorEventEndpoint::isWorking() const
{
    return m_isListening.load(std::memory_order_acquire);
}

void DetectorEventEndpoint::stop()
{
    if (!DetectorEventEndpoint::isWorking()) {
        return;
    }

    websocketpp::lib::error_code ec;
    m_deviceEventServer.stop_listening(ec);
    if (ec) {
        COMPLOG_ERROR("[WS] Error stopping server:", ec.message());
    }

    m_connectionMx.lock();
    for (auto& hdl : m_connections) {
        try {
            m_deviceEventServer.close(hdl, websocketpp::close::status::going_away, "Server shutdown");
        } catch (...) {
            // Игнорируем ошибки закрытия
        }
    }
    m_connections.clear();
    m_connectionMx.unlock();

    if (m_ioThread && m_ioThread->joinable()) {
        m_ioService.stop();
        m_ioThread->join();
        m_ioThread.reset();
    }

    m_isListening.store(false, std::memory_order_release);
}
