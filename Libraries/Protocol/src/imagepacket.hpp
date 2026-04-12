#pragma once

#include <vector>
#include <stdint.h>

namespace Protocol {

/**
 * @brief The ImagePacket class Part of image data (first or next packets)
 */
class ImagePacket
{
public:
    bool isValid() const;

    void setId(uint64_t id);
    void setTotalSize(uint64_t totalSize);

    void setFragmentStart(uint64_t startByte);
    uint64_t getFragmentStart() const;

    void setPayload(std::vector<uint8_t>&& payload);
    const std::vector<uint8_t>& getPayload() const;

    bool initFromPacketPart(const std::vector<uint8_t>& iData);
    std::vector<uint8_t> convertToPacketPart() const;

    bool operator <(const ImagePacket& _oPacket) const;

private:
    uint64_t                m_shotId {};
    uint64_t                m_totalSize {};
    uint64_t                m_fragmentStartByte {};
    std::vector<uint8_t>    m_payload {};
};

} // namespace Protocol
