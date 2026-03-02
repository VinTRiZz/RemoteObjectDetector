#pragma once

#include <Components/Network/ClientUDP.h>
#include <Components/ExtraClasses/DataFragmentator.h>

#include <string>
#include <stdint.h>

namespace ImageProcessing
{

class Streamer
{
public:
    Streamer();

    void sendImage(std::vector<uint8_t>&& imageData);

    void start(const std::string& serverHost, uint16_t serverPort);

private:
    UDP::Client m_streamClient;
    ExtraClasses::DataFragmentator m_imagesFragmentator;
};

}
