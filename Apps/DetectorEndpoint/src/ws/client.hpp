#pragma once

#include <memory>
#include <string>
#include <future>
#include <cstdint>

namespace WebSockets {

enum class DisconnectReason {
    Normal,           // нормальное закрытие
    ServerClosed,     // сервер закрыл соединение
    ConnectionLost,   // потеря соединения
    ProtocolError,    // ошибка протокола
    UserRequested     // отключено пользователем
};

class Client {
public:
    Client();
    ~Client();

    // Запрет копирования и перемещения
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;

    // Подключение к хосту и порту (формируется URI ws://host:port)
    void connect(const std::string& host, uint16_t port);

    // Проверка состояния соединения
    bool isConnected() const;

    // Отправка ping с полезной нагрузкой заданного размера (байт) и таймаутом в миллисекундах.
    // Возвращает future, которое получит значение:
    //   - положительное число (условное время ответа) при успехе;
    //   - -1 при таймауте или ошибке.
    std::future<int> ping(size_t bytes, int timeoutMs);

    // Отключение с указанием причины
    void disconnect(DisconnectReason reason);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace WebSockets

