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

    /**
     * @brief setDebugMode  In this mode, endpoint sends images from debug video to server
     * @param isDebug
     */
    void setDebugMode(bool isDebug);

    void setDeviceId(long long deviceId);

    bool start(const std::string &host, uint16_t streamPort, uint16_t eventPort);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void prepareShot();
    void sendShot();
};

