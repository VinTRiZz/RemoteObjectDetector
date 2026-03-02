#include "imagestreamer.hpp"

namespace ImageProcessing
{

Streamer::Streamer()
{

}

void Streamer::sendImage(std::vector<uint8_t> &&imageData)
{
    // TODO: Split data and send
}

void Streamer::start(const std::string &serverHost, uint16_t serverPort)
{
    m_streamClient.setHost(serverHost, serverPort);
}

}
