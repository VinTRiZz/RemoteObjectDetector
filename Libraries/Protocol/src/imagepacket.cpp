#include "imagepacket.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

#include <Components/Logger/Logger.h>

#include <ROD/ImageProcessing/Utility.h>

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

static bool s_isLoggingEnabled {false};

/**
 * @brief The vector_istreambuf class Extra class for reading data from std::vector
 */
struct vector_istreambuf : std::streambuf {
    vector_istreambuf(const std::vector<uint8_t>& v) {
        auto* data = reinterpret_cast<char*>(const_cast<uint8_t*>(v.data()));
        setg(data, data, data + v.size());
    }
};

/**
 * @brief The vector_ostreambuf class Extra class for writing data to std::vector
 */
struct vector_ostreambuf : std::streambuf {
    explicit vector_ostreambuf(std::vector<uint8_t>& v) : vec(v) {}

protected:
    int_type overflow(int_type c) override {
        if (c != traits_type::eof()) {
            vec.push_back(static_cast<uint8_t>(c));
        }
        return c;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override {
        vec.insert(vec.end(), s, s + n);
        return n;
    }

private:
    std::vector<uint8_t>& vec;
};

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

bool ImagePacket::initFromPacketPart(const ImageData_t &iData)
{
    if (iData.empty()) return false;

    vector_istreambuf isb(iData);
    std::istream is(&isb);
    try {
        boost::archive::binary_iarchive ia(is);
        ia >> *this;

        if (m_payload.size() > MTU_PAYLOAD_SIZE) {
            throw std::invalid_argument("Serialized payload size is more, than allowed");
        }

        return true;
    } catch (const boost::archive::archive_exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("(archivator error) Failed to deserialize packet:", e.what());
        }
    } catch (const std::exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("(Unknown error) Failed to deserialize packet:", e.what());
        }
    }
    *this = {}; // Reset self
    return false;
}

ImageData_t ImagePacket::convertToPacketPart() const
{
    ImageData_t oData;
    oData.reserve(MTU_SIZE);
    vector_ostreambuf os(oData);
    try {
        boost::archive::binary_oarchive oa(os);
        oa << *this;
        return oData;
    } catch (const boost::archive::archive_exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("(archivator error) Failed to deserialize packet:", e.what());
        }
    } catch (const std::exception& e) {
        if (s_isLoggingEnabled) {
            COMPLOG_WARNING("(Unknown error) Failed to deserialize packet:", e.what());
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
