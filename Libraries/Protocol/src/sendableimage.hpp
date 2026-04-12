#pragma once

#include <vector>
#include <stdint.h>
#include <string>

namespace Protocol {

/**
 * @brief The SendableImage class Class for UDP Send / receive data
 */
class SendableImage
{
public:
    bool isValid() const;

    void setImage(uint64_t imageId, std::vector<uint8_t>&& imgData);
    std::vector<uint8_t>& getImage();

    bool initFromPackets(std::vector<std::vector<uint8_t> >&& iPackets);
    std::vector<std::vector<uint8_t> > convertToPackets() const;

private:
    bool                    m_isValid {false};
    std::vector<uint8_t>    m_imageBytes;
    uint64_t                m_imageId {};

    std::vector<std::vector<uint8_t> > m_cachedPackets;
};

} // namespace Protocol
