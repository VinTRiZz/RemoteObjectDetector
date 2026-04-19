#pragma once

#include <vector>
#include <stdint.h>
#include <set>
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

    uint64_t getId() const;

    void setImage(uint64_t imageId, ImageProcessing::ImageData_t&& imgData);
    ImageProcessing::ImageData_t& getImage();

    bool canInitFrom(const std::set<ImagePacket>& iPackets) const;
    bool initFromPackets(std::vector<ImageProcessing::ImageData_t >&& iPackets);
    bool initFromPackets(std::set<ImagePacket>&& iPackets);
    std::vector<ImageProcessing::ImageData_t > convertToPackets() const;

    std::string_view getLastErrorText() const;

private:
    ImageProcessing::ImageData_t    m_imageBytes;
    uint64_t                        m_imageId {};
    uint64_t                        m_senderId {};

    // Error handling
    std::string m_lastErrorText;

    // Cache
    mutable bool m_imageChanged {true};
    mutable std::vector<ImageProcessing::ImageData_t> m_cachedPackets;
};

} // namespace Protocol
