#pragma once

#include <ROD/ImageProcessing.h>
#include "../streamer/imagestreamer.hpp"
#include "../camera/cameraadaptor.hpp"

#include <thread>

class DetectorEndpoint
{
public:
    DetectorEndpoint();
    ~DetectorEndpoint();

    ImageProcessing::Streamer& getStreamer();

private:
    Adaptors::CameraAdaptor m_camera;
    ImageProcessing::Processor m_imgProcessor {std::thread::hardware_concurrency()};
    ImageProcessing::Streamer m_imgStreamer;
};

