#include "detectorendpoint.hpp"

#include "eventendpoint.hpp"

#include <ROD/ImageProcessing/CameraAdaptor.h>
#include <ROD/ImageProcessing/ImageProcessor.h>
#include <ROD/ImageProcessing/VideoReader.h>
#include <ROD/Protocol.h>

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Network/ClientUDP.h>

#include <thread>
#include <atomic>

using namespace ImageProcessing;

struct DetectorEndpoint::Impl
{
    std::atomic<bool> isWorking {false};

    // Events
    EventEndpoint eventEndpoint;

    // Image processing
    CameraAdaptor                   camera;
    std::unique_ptr<VideoReader>    debugVideoReader;
    std::atomic<uint64_t>           pictureSendIntervalUs {1'000'000}; // Something like FPS

    // Streaming
    UDP::Client             streamingClient;
    VideoReader::Iterator   currentDebugShotIt;
    ImageData_t             currentShotData;

    uint64_t currentImageId {1};
};


DetectorEndpoint::DetectorEndpoint() :
    d {new Impl}
{

}

DetectorEndpoint::~DetectorEndpoint()
{
    stop();
}

void DetectorEndpoint::setDebugMode(bool isDebug)
{
    if (isDebug) {
        d->debugVideoReader = std::make_unique<ImageProcessing::VideoReader>();

        auto& dirManager = Common::DirectoryManager::getInstance();
        auto dataDir = dirManager.getDirectory(Common::DirectoryManager::Data);
        if (!d->debugVideoReader->setVideofile(dataDir / "translation_test.mp4")) {
            throw std::runtime_error("Did not found debug video item. Reinstall app, or copy nessesary file");
        }
        return;
    }

    d->debugVideoReader.reset();
}

void DetectorEndpoint::setDeviceId(long long deviceId)
{
    d->eventEndpoint.setDeviceId(deviceId);
}

bool DetectorEndpoint::start(const std::string &host, uint16_t streamPort, uint16_t eventPort)
{
    COMPLOG_INFO("Connecting to server...");
    d->eventEndpoint.setServer(host, eventPort);
    d->eventEndpoint.connect();
    d->streamingClient.setHost(host, streamPort);
    d->isWorking.store(true, std::memory_order_release);

    COMPLOG_INFO("Starting endpoint...");
    while (d->isWorking.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::microseconds(d->pictureSendIntervalUs));
        prepareShot();
        sendShot();
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

void DetectorEndpoint::prepareShot()
{
    if (d->debugVideoReader) {
        d->currentShotData = *d->currentDebugShotIt;
        ++d->currentDebugShotIt;
        if (d->currentDebugShotIt == d->debugVideoReader->end()) {
            d->currentDebugShotIt = d->debugVideoReader->begin(); // Restart debug streaming
        }
    } else {
        d->currentShotData = d->camera.shot();
    }
}

void DetectorEndpoint::sendShot()
{
    if (!d->eventEndpoint.isConnected()) {
        d->eventEndpoint.connect();
        return; // Skip current frame
    }

    if (d->eventEndpoint.isConnected()) {
        COMPLOG_DEBUG("Sending image to server...");

        Protocol::SendableImage img;
        img.setImage(d->currentImageId, std::move(d->currentShotData));
        d->currentImageId++;

        auto imagePackets = img.convertToPackets();
        for (auto& pkt : imagePackets) {
            if (!d->streamingClient.sendByteData(pkt)) {
                return;
            }
        }
        return;
    }

    // TODO: Process by myself, save into cache and try to reconnect
    COMPLOG_WARNING("Server disconnected, image skipped");
}
