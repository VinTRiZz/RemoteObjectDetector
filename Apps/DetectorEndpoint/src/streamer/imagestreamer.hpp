#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <vector>


/**
 * @brief The ImageStreamer class   Class to send data by UDP to the server
 */
class ImageStreamer
{
public:
    ImageStreamer();
    ~ImageStreamer();

    /**
     * @brief sendImage Splits image and sends to the host
     * @param imageData Any bytes, actually, according to server format
     */
    void sendImage(std::vector<uint8_t>&& imageData);

    /**
     * @brief setHost       Set host to send images
     * @param serverHost    Address in IPv4 format
     * @param serverPort
     */
    void setHost(const std::string& serverHost, uint16_t serverPort);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
