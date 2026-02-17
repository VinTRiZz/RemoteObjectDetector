#include "client.hpp"

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <mutex>
#include <map>
#include <cstring>
#include <iostream>

#include <Components/Logger/Logger.h>

namespace WebSockets {

// ========================== pImpl implementation ==========================
struct Client::Impl {
    using Client = websocketpp::client<websocketpp::config::asio_client>;
    using ConnectionHdl = websocketpp::connection_hdl;
    using MessagePtr = websocketpp::config::asio_client::message_type::ptr;

    // Члены класса (стиль m_)
    Client m_client;
    std::unique_ptr<std::thread> m_ioThread;
    websocketpp::lib::asio::io_service m_ioService;  // можно получить из client, но для ясности
    ConnectionHdl m_connection;
    bool m_connected = false;
    mutable std::mutex m_mutex;

    // Для обработки пингов
    struct PendingPing {
        std::promise<int> promise;
        std::unique_ptr<websocketpp::lib::asio::steady_timer> timer;
    };
    std::map<uint64_t, PendingPing> m_pendingPings;
    uint64_t m_nextPingId = 0;
    std::mutex m_pingMutex;

    Impl() {
        m_client.init_asio(&m_ioService);
        setHandlers();
    }

    ~Impl() {
        disconnect(DisconnectReason::Normal);
        if (m_ioThread && m_ioThread->joinable()) {
            m_ioService.stop();
            m_ioThread->join();
        }
    }

    void setHandlers() {
        m_client.set_open_handler([this](ConnectionHdl hdl) { onOpen(hdl); });
        m_client.set_message_handler([this](ConnectionHdl hdl, MessagePtr msg) { onMessage(hdl, msg); });
        m_client.set_close_handler([this](ConnectionHdl hdl) { onClose(hdl); });
        m_client.set_fail_handler([this](ConnectionHdl hdl) { onFail(hdl); });
        m_client.set_pong_handler([this](ConnectionHdl hdl, std::string payload) { onPong(hdl, payload); });
    }

    void onOpen(ConnectionHdl hdl) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_connection = hdl;
            m_connected = true;
        }
        COMPLOG_OK("Connected to server");

        // Отправка двух обязательных сообщений: текст и JSON
        try {
            m_client.send(hdl, "hello", websocketpp::frame::opcode::text);
            nlohmann::json j;
            j["test"] = "hello";
            m_client.send(hdl, j.dump(), websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            COMPLOG_ERROR("Failed to send initial messages:", e.what());
        }
    }

    void onMessage(ConnectionHdl /*hdl*/, MessagePtr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            std::string payload = msg->get_payload();
            COMPLOG_INFO("Text got:", payload);
            // Попытка интерпретировать как JSON и извлечь поле "test"
            try {
                auto j = nlohmann::json::parse(payload);
                if (j.contains("test")) {
                    std::string testVal = j["test"];
                    COMPLOG_INFO("Field test:", testVal);
                }
            } catch (...) {
                // Не JSON – игнорируем
            }
        } else {
            COMPLOG_INFO("Binary message received, size:", msg->get_payload().size());
        }
    }

    void onClose(ConnectionHdl hdl) {
        DisconnectReason reason = DisconnectReason::ServerClosed; // по умолчанию
        websocketpp::close::status::value code;
        std::string reasonStr;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto con = m_client.get_con_from_hdl(hdl);
            code = con->get_remote_close_code();
            reasonStr = con->get_remote_close_reason();
            m_connected = false;
        }
        // Преобразование кода закрытия в enum DisconnectReason (упрощённо)
        if (code == websocketpp::close::status::normal) {
            reason = DisconnectReason::Normal;
        } else if (code == websocketpp::close::status::going_away) {
            reason = DisconnectReason::ServerClosed;
        } else if (code == websocketpp::close::status::protocol_error) {
            reason = DisconnectReason::ProtocolError;
        } else {
            reason = DisconnectReason::ConnectionLost;
        }
        COMPLOG_WARNING("Disconnected:", reasonStr, "code:", code);
    }

    void onFail(ConnectionHdl hdl) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_connected = false;
        }
        auto con = m_client.get_con_from_hdl(hdl);
        auto ec = con->get_ec();
        COMPLOG_ERROR("Connection failed:", ec.message());
    }

    void onPong(ConnectionHdl /*hdl*/, std::string payload) {
        if (payload.size() >= sizeof(uint64_t)) {
            uint64_t id;
            std::memcpy(&id, payload.data(), sizeof(id));
            std::lock_guard<std::mutex> lock(m_pingMutex);
            auto it = m_pendingPings.find(id);
            if (it != m_pendingPings.end()) {
                it->second.promise.set_value(1); // здесь можно вычислить RTT, но для примера ставим 1
                it->second.timer->cancel();
                m_pendingPings.erase(it);
            }
        }
    }

    void disconnect(DisconnectReason reason) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_connected) return;

        websocketpp::close::status::value code = websocketpp::close::status::normal;
        switch (reason) {
            case DisconnectReason::Normal:        code = websocketpp::close::status::normal; break;
            case DisconnectReason::ServerClosed:  code = websocketpp::close::status::going_away; break;
            case DisconnectReason::ProtocolError: code = websocketpp::close::status::protocol_error; break;
            case DisconnectReason::ConnectionLost:code = websocketpp::close::status::abnormal_close; break;
            case DisconnectReason::UserRequested: code = websocketpp::close::status::normal; break;
        }

        try {
            m_client.close(m_connection, code, "Disconnect by user");
        } catch (const std::exception& e) {
            COMPLOG_ERROR("Error during disconnect:", e.what());
        }
        m_connected = false;
    }
};

// ========================== Client ==========================
Client::Client() : d(std::make_unique<Impl>()) {}

Client::~Client() = default;

void Client::connect(const std::string& host, uint16_t port) {
    std::string uri = "ws://" + host + ":" + std::to_string(port);
    websocketpp::lib::error_code ec;
    auto con = d->m_client.get_connection(uri, ec);
    if (ec) {
        COMPLOG_ERROR("Connection error:", ec.message());
        return;
    }
    d->m_client.connect(con);
    // Запуск потока обработки событий, если ещё не запущен
    if (!d->m_ioThread) {
        d->m_ioThread = std::make_unique<std::thread>([this]() {
            d->m_client.run();
        });
    }
}

bool Client::isConnected() const {
    std::lock_guard<std::mutex> lock(d->m_mutex);
    return d->m_connected;
}

std::future<int> Client::ping(size_t bytes, int timeoutMs) {
    // Проверка соединения
    if (!isConnected()) {
        std::promise<int> p;
        p.set_value(-1);
        return p.get_future();
    }

    uint64_t id;
    std::promise<int> promise;
    auto future = promise.get_future();

    {
        std::lock_guard<std::mutex> lock(d->m_pingMutex);
        id = d->m_nextPingId++;
        auto& pending = d->m_pendingPings[id];
        pending.promise = std::move(promise);
        pending.timer = std::make_unique<websocketpp::lib::asio::steady_timer>(d->m_ioService);
        pending.timer->expires_after(std::chrono::milliseconds(timeoutMs));
        pending.timer->async_wait([this, id](const websocketpp::lib::asio::error_code& ec) {
            if (ec) return; // таймер был отменён
            std::lock_guard<std::mutex> lock(d->m_pingMutex);
            auto it = d->m_pendingPings.find(id);
            if (it != d->m_pendingPings.end()) {
                it->second.promise.set_value(-1); // таймаут
                d->m_pendingPings.erase(it);
            }
        });
    }

    // Формирование payload: первые 8 байт — идентификатор, остальное — нули
    std::string payload(bytes, '\0');
    if (bytes >= sizeof(id)) {
        std::memcpy(&payload[0], &id, sizeof(id));
    } else {
        // Если размер слишком мал, просто отправляем нули – идентификатор не сохранится, но так редко бывает
        // В реальном проекте можно обработать ошибку
    }

    websocketpp::lib::error_code ec;
    d->m_client.ping(d->m_connection, payload, ec);
    if (ec) {
        // Ошибка отправки – сразу возвращаем -1
        std::lock_guard<std::mutex> lock(d->m_pingMutex);
        auto it = d->m_pendingPings.find(id);
        if (it != d->m_pendingPings.end()) {
            it->second.promise.set_value(-1);
            d->m_pendingPings.erase(it);
        }
    }

    return future;
}

void Client::disconnect(DisconnectReason reason) {
    d->disconnect(reason);
}

} // namespace WebSockets
