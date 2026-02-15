#pragma once

#include <memory>
#include <stdint.h>

/**
 * @brief The ServerEndpoint class Главный объект сервера
 */
class ServerEndpoint
{
public:
    ServerEndpoint();
    ~ServerEndpoint();

    void start(uint16_t wsEventPort, uint16_t httpAPIPort, uint16_t udpStreamingPort);
    bool isWorking() const;
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

