#pragma once

#include <vector>
#include <stdint.h>

#include <ROD/ImageProcessing/Common.h>

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

    void setPayload(ImageProcessing::ImageData_t&& payload);
    const ImageProcessing::ImageData_t& getPayload() const;

    bool initFromPacketPart(const ImageProcessing::ImageData_t& iData);
    ImageProcessing::ImageData_t convertToPacketPart() const;

    bool operator <(const ImagePacket& _oPacket) const;

private:
    uint64_t                m_shotId {};
    uint64_t                m_totalSize {};
    uint64_t                m_fragmentStartByte {};
    ImageProcessing::ImageData_t m_payload {};
};

} // namespace Protocol
