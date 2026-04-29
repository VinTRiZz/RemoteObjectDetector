#include "imagepacket.hpp"

#include <Components/Logger/Logger.h>

#include <ROD/ImageProcessing/Utility.h>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

using namespace ImageProcessing;

/*

TOTAL SIZE: 1400 (Ethernet MTU)
PAYLOAD: Calculates using other parts of a packet

Shot ID 0 is reserved for invalid shots


Field name          |  Size (byte)  |    Example    |   Description
--------------------------------------------------------------------------------------------------------------------
Shot ID             |       8       |  00000000001  |   Sequential, from 1 to N, increments every time shot created
Fragment start      |       8       |  00000000000  |   Always zero (conversion issue)
Shot hash           |      64       |  a5317f9123e  |   XXH3 hash
Total size          |       8       |  00000000001  |   Image bytes total count
Payload             |   Calculated  |  ...........  |   Image bytes


*/


namespace Protocol {

/**
 * @brief serialize Serialization for bitsery
 * @param s
 * @param o
 */
template <typename Archivator>
void ImagePacket::serialize(Archivator& archivator) {
    archivator.value8b(m_senderId);
    archivator.value8b(m_shotId);
    archivator.value8b(m_totalImageSize);
    archivator.value8b(m_fragmentStartByte);
    archivator.container1b(m_imageHash, 64);
    archivator.container1b(m_payload, MTU_PAYLOAD_SIZE);
}

static bool s_isLoggingEnabled {false};

void ImagePacket::setLoggingEnabled(bool isLoggingEnabled)
{
    s_isLoggingEnabled = isLoggingEnabled;
}

bool ImagePacket::isValid() const
{
    return (m_shotId == 0);
}

void ImagePacket::setId(uint64_t id)
{
    m_shotId = id;
}

uint64_t ImagePacket::getId() const
{
    return m_shotId;
}

void ImagePacket::setSenderId(uint64_t sId)
{
    m_senderId = sId;
}

uint64_t ImagePacket::getSenderId() const
{
    return m_senderId;
}

void ImagePacket::setTotalImageSize(uint64_t totalSize)
{
    if (m_fragmentStartByte > totalSize) {
        m_fragmentStartByte = 0; // Reset in case of validance
    }
    m_totalImageSize = totalSize;
}

uint64_t ImagePacket::getTotalImageSize() const
{
    return m_totalImageSize;
}

void ImagePacket::setFragmentStart(uint64_t startByte)
{
    if (startByte > m_totalImageSize) {
        throw std::invalid_argument("Invalid start byte (out of bounds)");
    }
    m_fragmentStartByte = startByte;
}

uint64_t ImagePacket::getFragmentStart() const
{
    return m_fragmentStartByte;
}

void ImagePacket::setImageHash(const std::vector<uint8_t> &imgHash)
{
    m_imageHash = imgHash;
}

const std::vector<uint8_t> &ImagePacket::getImageHash() const
{
    return m_imageHash;
}

void ImagePacket::setPayload(ImageData_t &&payload)
{
    if (payload.size() > MTU_PAYLOAD_SIZE) {
        throw std::invalid_argument("Payload size is more, than allowed");
    }
    m_payload = std::move(payload);
}

const ImageData_t &ImagePacket::getPayload() const
{
    return m_payload;
}

/*
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & m_senderId;
        ar & m_shotId;
        ar & m_fragmentStartByte;

        // TODO: Separate fields if good solution
        ar & m_imageHash;
        ar & m_totalImageSize;

        ar & m_payload;
    }
*/

bool ImagePacket::initFromPacketPart(const std::vector<uint8_t> &iData)
{
    if (iData.empty()) return false;
    try {
        auto state = bitsery::quickDeserialization<bitsery::InputBufferAdapter<std::vector<uint8_t> > >({iData.begin(), iData.size()}, *this);

        if (state.first != bitsery::ReaderError::NoError || state.second) {
            COMPLOG_WARNING("Failed to deserialize packet:", static_cast<int>(state.first));
            return false;
        }

        if (m_payload.size() > MTU_PAYLOAD_SIZE) {
            throw std::invalid_argument("Serialized payload size is more, than allowed");
        }

        return true;
    } catch (const std::exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("Failed to deserialize packet:", e.what());
        }
    }
    *this = {}; // Reset self
    return false;
}

std::vector<uint8_t> ImagePacket::convertToPacketPart() const
{
    std::vector<uint8_t> oData;
    oData.reserve(MTU_SIZE);
    try {
        auto writtenSize = bitsery::quickSerialization<bitsery::OutputBufferAdapter<std::vector<uint8_t>>>(oData, *this);
        if (writtenSize < 1) {
            return {};
        }
        return oData;
    } catch (const std::exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("Failed to deserialize packet:", e.what());
        }
    }
    return {};
}

bool ImagePacket::operator <(const ImagePacket& _oPacket) const {
    return m_fragmentStartByte < _oPacket.m_fragmentStartByte;
}

bool ImagePacket::operator ==(const ImagePacket& _oPacket) const {
    return
        m_senderId          == _oPacket.m_senderId &&
        m_shotId            == _oPacket.m_shotId &&
        m_totalImageSize    == _oPacket.m_totalImageSize &&
        m_fragmentStartByte == _oPacket.m_fragmentStartByte &&
        m_imageHash         == _oPacket.m_imageHash &&
        m_payload           == _oPacket.m_payload;
}

bool ImagePacket::operator !=(const ImagePacket& _oPacket) const {
    return !(*this == _oPacket);
}

} // namespace Protocol
