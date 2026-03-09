#pragma once

#include <memory>
#include <string>

/**
 * @brief The DetectorEndpoint class    Main instance of detector
 */
class DetectorEndpoint
{
public:
    DetectorEndpoint();
    ~DetectorEndpoint();

    void setToken(const std::string& tokenString);

    bool start(const std::string &host, uint16_t streamPort, uint16_t eventPort);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

