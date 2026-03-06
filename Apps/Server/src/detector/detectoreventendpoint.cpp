#include "detectoreventendpoint.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Encryption/Encoding.h>

#include <ROD/Protocol.h>

#include <regex>

DetectorEventEndpoint::DetectorEventEndpoint(Protocol::EventProcessor &serverEventProcessor, Protocol::EventProcessor &deviceEventProcessor) :
    AbstractEndpoint(serverEventProcessor),
    m_deviceEventProcessor {deviceEventProcessor}
{
    m_deviceEventServer.init_asio(&m_ioService);

    // Common settings
    m_deviceEventServer.set_reuse_addr(true);
    initConnectionCallbacks();
    m_deviceEventServer.set_http_handler([this](ConnectionHdl hdl) {
        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        con->set_body("Invalid protocol (WebSockets required)");
        con->set_status(websocketpp::http::status_code::not_found);
    });

    // Message processing
    m_deviceEventServer.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) {
        if (msg->get_opcode() != websocketpp::frame::opcode::text) {
            COMPLOG_WARNING("[WS] Received binary data (skipped)");
            return;
        }
        Protocol::Event ev;
        if (!ev.readRaw(msg->get_payload())) {
            COMPLOG_ERROR(Protocol::toString(ev.type), msg->get_payload());
            return;
        }
        COMPLOG_DEBUG(Protocol::toString(ev.type), msg->get_payload());
        m_deviceEventProcessor.addEvent(std::move(ev));
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
            m_deviceEventServer.close(hdl.first, websocketpp::close::status::going_away, "Server shutdown");
        } catch (const std::exception& ex) {
            COMPLOG_WARNING("Close exception:", ex.what());
        } catch (...) {
            COMPLOG_WARNING("Unknown close exception");
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

void DetectorEventEndpoint::initConnectionCallbacks()
{
    m_deviceEventServer.set_validate_handler([this](ConnectionHdl hdl) -> bool {
        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        auto remote = con->get_remote_endpoint();

        auto parameters = con->get_resource();
        std::regex devnameRegexp("\\/[?&]dev=([a-f0-9]+)");
        std::smatch devnameMatch;
        if (!std::regex_match(parameters, devnameMatch, devnameRegexp)) {
            COMPLOG_WARNING("[INVALID PROTOCOL] Rejected connection from:", remote);
            return false;
        }

        auto deviceName = devnameMatch[1].str();
        if (!isDevValid(deviceName)) {
            COMPLOG_WARNING("[INVALID DEVNAME] Rejected connection from:", remote);
            return false;
        }
        m_connectionMx.lock();
        m_connections.emplace(hdl, deviceName);
        m_connectionMx.unlock();

        COMPLOG_OK("[WS] Client connected:", remote);

        Protocol::Event ev;
        ev.type = Protocol::EventType::DetectorConnected;
        ev.device = m_connections[hdl];
        m_deviceEventProcessor.addEvent(std::move(ev));
        return true;
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

        Protocol::Event ev;
        ev.type = Protocol::EventType::DetectorDisconnected;
        ev.device = m_connections[hdl];
        ev.payload = Encryption::encodeHex(reasonStr);
        m_deviceEventProcessor.addEvent(std::move(ev));
    });

    m_deviceEventServer.set_fail_handler([this](ConnectionHdl hdl) {
        auto con = m_deviceEventServer.get_con_from_hdl(hdl);
        auto ec = con->get_ec();
        COMPLOG_ERROR("[WS] Connection:", ec.message());
    });
}

bool DetectorEventEndpoint::isDevValid(const std::string &devname) const
{
    if ((devname.empty() ||
        devname.size() != 64) && // Device hash (token)
        (devname != "MANAGER")) {
        COMPLOG_ERROR("Here", devname.size(), devname);
            return false;
    }

    if (devname == "MANAGER") {
        std::lock_guard<std::mutex> lock(m_connectionMx);
        if (m_isManagerAuthorized) {
            return false;
        }
    } else {
        // TODO: Check devices id DB
        COMPLOG_WARNING("DEVICE TOKEN CHECK IGNORED");
    }
    return true;
}
