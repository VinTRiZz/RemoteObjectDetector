#include "imagepacket.hpp"

/*

TOTAL SIZE: 1400 (Ethernet MTU)
PAYLOAD: Calculates using other parts of a packet

Shot ID 0 is reserved for invalid shots


==========> FIRST PACKET:

Field name          |  Size (byte)  |    Example    |   Description
--------------------------------------------------------------------------------------------------------------------
Fragment start      |       8       |  00000000001  |   Byte offset of a fragment, also identify first packet
Shot ID             |       8       |  00000000001  |   Sequential, from 1 to N, increments every time shot created
Shot hash           |      64       |  a5317f9123e  |   XXH3 hash
Total size          |       8       |  00000000001  |   Image bytes total count
Payload             |   Calculated  |  ...........  |   Image bytes


==========> MIDDLE AND LAST PACKET:

Field name          |  Size (byte)  |    Example    |   Description
--------------------------------------------------------------------------------------------------------------------
Fragment start      |       8       |  00000000001  |   Byte offset of a fragment, also identify first packet
Shot ID             |       8       |  00000000001  |   Sequential, from 1 to N, increments every time shot created
Payload             |   Calculated  |  ...........  |   Image bytes

*/

namespace Protocol {

bool ImagePacket::isValid() const
{
    return (m_shotId == 0);
}

void ImagePacket::setId(uint64_t id)
{
    m_shotId = id;
}

void ImagePacket::setTotalSize(uint64_t totalSize)
{
    m_totalSize = totalSize;
}

void ImagePacket::setFragmentStart(uint64_t startByte)
{
    m_fragmentStartByte = startByte;
}

uint64_t ImagePacket::getFragmentStart() const
{
    return m_fragmentStartByte;
}

void ImagePacket::setPayload(std::vector<uint8_t> &&payload)
{
    m_payload = std::move(payload);
}

const std::vector<uint8_t> &ImagePacket::getPayload() const
{
    return m_payload;
}

bool ImagePacket::initFromPacketPart(const std::vector<uint8_t> &iData)
{
    return false;
}

std::vector<uint8_t> ImagePacket::convertToPacketPart() const
{
    return {};
}

bool ImagePacket::operator <(const ImagePacket& _oPacket) const {
    return m_fragmentStartByte < _oPacket.m_fragmentStartByte;
}



} // namespace Protocol
