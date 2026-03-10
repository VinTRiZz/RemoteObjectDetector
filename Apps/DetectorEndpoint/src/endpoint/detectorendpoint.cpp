#include "detectorendpoint.hpp"

#include <ROD/CameraAdaptor.h>
#include "eventendpoint.hpp"

#include <ROD/ImageProcessing.h>

#include <Components/Logger/Logger.h>

#include <thread>
#include <atomic>

struct DetectorEndpoint::Impl
{
    std::atomic<bool> isWorking {false};

    // Events
    EventEndpoint eventEndpoint;

    // Image processing things
    ImageProcessing::CameraAdaptor  camera;
    ImageProcessing::Processor      imgProcessor {std::thread::hardware_concurrency()};
};


DetectorEndpoint::DetectorEndpoint() :
    d {new Impl}
{

}

DetectorEndpoint::~DetectorEndpoint()
{
    stop();
}

void DetectorEndpoint::setToken(const std::string &tokenString)
{
    d->eventEndpoint.setToken(tokenString);
}

bool DetectorEndpoint::start(const std::string &host, uint16_t streamPort, uint16_t eventPort)
{
    COMPLOG_INFO("Connecting to server...");
    d->eventEndpoint.connect(host, eventPort);
    if (!d->camera.initStreamingSimple(host, streamPort)) {
        return false;
    }

    COMPLOG_INFO("Starting endpoint...");
    while (d->isWorking.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (d->eventEndpoint.isConnected()) {
            // TODO: Send for processing
            COMPLOG_DEBUG("Sending to server...");
        } else {
            // TODO: Process by myself and try to reconnect
            COMPLOG_DEBUG("Processing by myself...");
        }
    }
    COMPLOG_INFO("Endpoint stopped");
    return true;
}

void DetectorEndpoint::stop()
{
    if (!d->isWorking.load(std::memory_order_acquire)) {
        return;
    }
    d->isWorking.store(false, std::memory_order_release);
    d->eventEndpoint.disconnect();
}
