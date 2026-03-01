#pragma once

#include <memory>
#include <stdint.h>
#include <string>

#include "servercommon.hpp"

/**
 * @brief The ServerEndpoint class Главный объект сервера
 */
class ServerEndpoint
{
public:
    ServerEndpoint(const std::string& dbPath);
    ~ServerEndpoint();

    void start(uint16_t wsEventPort, uint16_t httpAPIPort, uint16_t udpStreamingPort);
    bool isWorking() const;
    void stop();

private:
    std::string m_dbPath;
    struct Impl;
    std::unique_ptr<Impl> d;
};

