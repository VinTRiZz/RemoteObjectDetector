#include "eventendpoint.hpp"

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <nlohmann/json.hpp>
#include <thread>

#include <Components/Logger/Logger.h>

using Client = websocketpp::client<websocketpp::config::asio_client>;
using ConnectionHdl = websocketpp::connection_hdl;
using MessagePtr = websocketpp::config::asio_client::message_type::ptr;

struct EventEndpoint::Impl
{
    // WebSockets things
    Client                              eventClient;
    websocketpp::lib::asio::io_service  ioService;
    ConnectionHdl                       eventConnection;
    std::atomic<bool>                   connected {false};
};


EventEndpoint::EventEndpoint() :
    d {new Impl}
{
    d->eventClient.init_asio(&d->ioService);

    d->eventClient.set_open_handler([this](ConnectionHdl hdl) {
        d->eventConnection = hdl;
        d->connected.store(true, std::memory_order_release);
        COMPLOG_OK("[WS] Connected to server");
    });
    // d->eventClient.send(hdl, "hello", websocketpp::frame::opcode::text);

    d->eventClient.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            std::string payload = msg->get_payload();
            COMPLOG_DEBUG("[WS] Text got:", payload);
            Protocol::Event ev(payload);
            m_eventProcessor.addEvent(std::move(ev));
            return;
        }

        COMPLOG_WARNING("[WS] Binary message received, size:", msg->get_payload().size());
    });


    d->eventClient.set_close_handler([this](ConnectionHdl hdl) {
        websocketpp::close::status::value code;
        std::string reasonStr;
        auto pCon = d->eventClient.get_con_from_hdl(hdl);
        code = pCon->get_remote_close_code();
        reasonStr = pCon->get_remote_close_reason();
        d->connected.store(false, std::memory_order_release);
        COMPLOG_WARNING("[WS] Disconnected:", reasonStr, "code:", code);
    });


    d->eventClient.set_fail_handler([this](ConnectionHdl hdl) {
        d->connected.store(false, std::memory_order_release);
        auto pCon = d->eventClient.get_con_from_hdl(hdl);
        auto ec = pCon->get_ec();
        COMPLOG_ERROR("[WS] Failed to connect:", ec.message());
    });
}

EventEndpoint::~EventEndpoint()
{
    disconnect();
}

void EventEndpoint::setToken(const std::string &token)
{
    m_token = token;
}

void EventEndpoint::connect(const std::string &serverHost, uint16_t eventPort)
{
    std::string uri = "ws://" + serverHost + ":" + std::to_string(eventPort) + "/?dev=" + m_token;
    COMPLOG_SYNC_DEBUG(uri);
    websocketpp::lib::error_code ec;
    auto con = d->eventClient.get_connection(uri, ec);
    if (ec) {
        COMPLOG_ERROR("[WS] Connection:", ec.message());
        return;
    }
    d->eventClient.connect(con);
    d->eventClient.run();
}

bool EventEndpoint::isConnected() const
{
    return d->connected.load(std::memory_order_acquire);
}

void EventEndpoint::disconnect()
{
    if (!isConnected()) {
        return;
    }
    d->eventClient.close(d->eventConnection, websocketpp::close::status::normal, "Disconnected by user");
    d->connected.store(false, std::memory_order_release);
    COMPLOG_INFO("Disconnected from event channel");
}

ServerCommandProcessor &EventEndpoint::getEventProcessor()
{
    return m_eventProcessor;
}
