#pragma once

#include <vector>
#include <stdint.h>

#include <ROD/ImageProcessing/Common.h>

namespace boost::serialization {
class access;
}

namespace Protocol {

/**
 * @brief The ImagePacket class Part of image data (first or next packets)
 */
class ImagePacket
{
public:
    static void setLoggingEnabled(bool isLoggingEnabled);

    bool isValid() const;

    void setId(uint64_t id);
    uint64_t getId() const;

    void setTotalImageSize(uint64_t totalSize);
    uint64_t getTotalImageSize() const;

    void setFragmentStart(uint64_t startByte);
    uint64_t getFragmentStart() const;

    void setImageHash(const std::vector<uint8_t>& imgHash);
    const std::vector<uint8_t>& getImageHash() const;

    void setPayload(ImageProcessing::ImageData_t&& payload);
    const ImageProcessing::ImageData_t& getPayload() const;

    bool initFromPacketPart(const ImageProcessing::ImageData_t& iData);
    ImageProcessing::ImageData_t convertToPacketPart() const;

    bool operator <(const ImagePacket& _oPacket) const;
    bool operator ==(const ImagePacket& _oPacket) const;
    bool operator !=(const ImagePacket& _oPacket) const;

private:
    uint64_t                        m_shotId {};
    uint64_t                        m_totalImageSize {};
    uint64_t                        m_fragmentStartByte {};
    std::vector<uint8_t>            m_imageHash {};
    ImageProcessing::ImageData_t    m_payload {};

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & m_shotId;
        ar & m_fragmentStartByte;

        // TODO: Separate fields if good solution
        ar & m_imageHash;
        ar & m_totalImageSize;

        ar & m_payload;
    }
    friend class boost::serialization::access;

public:
    // UDP Minimal Transporting Unit sizes
    // TODO: Move to constants?
    static constexpr auto MTU_SIZE {1400};
    static constexpr auto MTU_PAYLOAD_SIZE {MTU_SIZE // TODO: Add processing for first packet (no sense in extra fields)
        - sizeof(m_shotId)
        - sizeof(m_fragmentStartByte)
        - sizeof(m_totalImageSize)
        - sizeof(m_imageHash)
    };
};

} // namespace Protocol
