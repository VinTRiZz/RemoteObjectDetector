#include "detectorendpoint.hpp"

#include <ROD/ImageProcessing.h>
#include "../streamer/imagestreamer.hpp"
#include "../camera/cameraadaptor.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Network/ClientWS.h>

#include <nlohmann/json.hpp>

// DEBUG
#include <thread>

namespace {

std::string createSimpleEvent(std::string&& eventText) {
    nlohmann::json js;
    js["event"] = eventText;
    return js.dump();
}

std::string getServerRequest(const std::string& rawJson) {
    try {
        auto serverReq = nlohmann::json::parse(rawJson);
        return serverReq["action"];
    } catch (nlohmann::json& ex) {
        return {};
    }
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

    d->eventStreamer.setReceiveCallback([this](auto&& stringData){
        auto serverRequest = getServerRequest(stringData);

        // TODO: Parse process server request
        COMPLOG_DEBUG("Got request from server:", serverRequest, "(JSON:", stringData, ")");
    });
}

DetectorEndpoint::~DetectorEndpoint()
{
    stop();
}

void DetectorEndpoint::start()
{
    COMPLOG_INFO("Starting endpoint...");
    d->eventStreamer.connect(d->host, d->eventPort);
    if (!d->eventStreamer.isConnected()) {
        COMPLOG_ERROR("Failed to setup event stream");
        return;
    }
    d->eventStreamer.sendText(createSimpleEvent("start"));
    d->imgStreamer.setHost(d->host, d->streamPort);

    // TODO: Start sending loop with check if connected. If disconnected, process by self, and send result later
    while (d->eventStreamer.isConnected()) {
        COMPLOG_DEBUG("Processing...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
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
