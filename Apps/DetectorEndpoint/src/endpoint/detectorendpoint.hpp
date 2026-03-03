#pragma once

#include <memory>
#include <string>

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

