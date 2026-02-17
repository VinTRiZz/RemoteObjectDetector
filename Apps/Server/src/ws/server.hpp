#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace WebSockets {

enum class DisconnectReason {
    Normal,           // нормальное закрытие клиентом
    ServerShutdown,   // сервер остановлен
    ProtocolError,    // ошибка протокола
    ConnectionLost,   // потеря соединения
    Unknown
};

class Server {
public:
    Server();
    ~Server();

    // Запрет копирования и перемещения
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    // Запуск сервера на указанном хосте и порту
    void listen(const std::string& host, uint16_t port);

    // Остановка сервера и закрытие всех соединений
    void stop();

    // Проверка состояния (активно ли прослушивание)
    bool isListening() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};


} // namespace WebSockets
