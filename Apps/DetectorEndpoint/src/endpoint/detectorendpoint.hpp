#pragma once

#include <memory>
#include <string>

/**
 * @brief The DetectorEndpoint class    Main instance of detector
 */
class DetectorEndpoint
{
public:
    DetectorEndpoint(const std::string& host, uint16_t streamPort, uint16_t eventPort);
    ~DetectorEndpoint();

    void start();
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

