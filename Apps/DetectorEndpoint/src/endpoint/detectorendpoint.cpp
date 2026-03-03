#include "detectorendpoint.hpp"

#include <ROD/ImageProcessing.h>
#include "../streamer/imagestreamer.hpp"
#include "../camera/cameraadaptor.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Network/ClientWS.h>

#include <nlohmann/json.hpp>

namespace {

std::string createSimpleEvent(std::string&& eventText) {
    nlohmann::json js;
    js["event"] = eventText;
    return js.dump();
}

}

struct DetectorEndpoint::Impl
{
    std::string host {};
    uint16_t    streamPort {};
    uint16_t    eventPort {};

    Adaptors::CameraAdaptor     camera;
    ImageProcessing::Processor  imgProcessor {std::thread::hardware_concurrency()};

    WebSockets::Client  eventStreamer;
    ImageStreamer       imgStreamer;
};


DetectorEndpoint::DetectorEndpoint(const std::string &host, uint16_t streamPort, uint16_t eventPort) :
    d {new Impl}
{
    d->host = host;
    d->streamPort = streamPort;
    d->eventPort = eventPort;
}

DetectorEndpoint::~DetectorEndpoint()
{
    stop();
}

void DetectorEndpoint::start()
{
    d->eventStreamer.connect(d->host, d->eventPort);
    if (!d->eventStreamer.isConnected()) {
        COMPLOG_ERROR("Failed to setup event stream");
        return;
    }
    d->eventStreamer.sendText(createSimpleEvent("start"));

    d->imgStreamer.setHost(d->host, d->streamPort);

    COMPLOG_INFO("Starting image streamer...");
    // TODO: Start sending loop with check if connected. If disconnected, process by self, and send result later
}

void DetectorEndpoint::stop()
{
    if (!d->eventStreamer.isConnected()) {
        return;
    }
    d->eventStreamer.sendText(createSimpleEvent("stop"));
    d->eventStreamer.disconnect();
    COMPLOG_INFO("Stopped endpoint");
}
