#include "sendableimage.hpp"

#include "imagepacket.hpp"

#include <algorithm>
#include <set>

namespace Protocol {

using namespace ImageProcessing;

bool SendableImage::isValid() const
{
    return m_isValid;
}

void SendableImage::setImage(uint64_t imageId, ImageData_t &&imgData)
{
    m_cachedPackets.clear();
    m_imageBytes = std::move(imgData);
}

ImageData_t &SendableImage::getImage()
{
    return m_imageBytes;
}

bool SendableImage::initFromPackets(std::vector<ImageData_t > &&iPackets)
{
    std::set<ImagePacket> readPackets;

    uint64_t maxPacketSize {};
    for (auto& ip : iPackets) {
        ImagePacket imgPart;
        if (!imgPart.initFromPacketPart(ip)) {
            return false;
        }
        maxPacketSize = std::max(maxPacketSize, imgPart.getPayload().size());
        readPackets.emplace(std::move(imgPart));
    }

    m_imageBytes.reserve(readPackets.size() * maxPacketSize);
    for (auto& rp : readPackets) {
        auto& rpPayload = rp.getPayload();
        std::copy(rpPayload.begin(), rpPayload.end(), std::back_inserter(m_imageBytes));
    }
    m_imageBytes.shrink_to_fit();
    return true;
}

std::vector<ImageData_t > SendableImage::convertToPackets() const
{
    if (!m_cachedPackets.empty()) {
        return m_cachedPackets;
    }

    return m_cachedPackets;
}

} // namespace Protocol
