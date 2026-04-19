#pragma once

#include <vector>
#include <stdint.h>
#include <string>

#include <ROD/ImageProcessing/Common.h>
#include "imagepacket.hpp"

namespace Protocol {

/**
 * @brief The SendableImage class Class for UDP Send / receive data
 */
class SendableImage
{
public:
    bool isValid() const;

    void setSenderId(uint64_t sId);
    uint64_t getSenderId() const;

    void setImage(uint64_t imageId, ImageProcessing::ImageData_t&& imgData);
    ImageProcessing::ImageData_t& getImage();

    bool initFromPackets(std::vector<ImageProcessing::ImageData_t >&& iPackets);
    std::vector<ImageProcessing::ImageData_t > convertToPackets() const;

private:
    ImageProcessing::ImageData_t    m_imageBytes;
    uint64_t                        m_imageId {};
    uint64_t                        m_senderId {};

    // Cache
    mutable bool m_imageChanged {true};
    mutable std::vector<ImageProcessing::ImageData_t> m_cachedPackets;
};

} // namespace Protocol
