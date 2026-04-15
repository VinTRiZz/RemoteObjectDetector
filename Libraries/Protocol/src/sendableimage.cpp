#include "sendableimage.hpp"

#include "imagepacket.hpp"

#include <algorithm>
#include <set>

#include <Components/ExtraClasses/DataFragmentator.h>
#include <ROD/ImageProcessing/Utility.h>

namespace Protocol {

using namespace ImageProcessing;

bool SendableImage::isValid() const
{
    return m_isValid;
}

void SendableImage::setImage(uint64_t imageId, ImageData_t &&imgData)
{
    m_imageBytes = std::move(imgData);
    m_imageChanged = true;
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

    m_imageChanged = true;
    return true;
}

std::vector<ImageData_t > SendableImage::convertToPackets() const
{
    if (!m_imageChanged) {
        return m_cachedPackets;
    }
    auto imgHash = ImageProcessing::Utility::calculateImageHash(m_imageBytes);

    auto dataPackets = ExtraClasses::DataInfo::split(m_imageBytes, ImagePacket::MTU_PAYLOAD_SIZE);
    m_cachedPackets.clear();
    m_cachedPackets.reserve(dataPackets.size());

    ImagePacket imgP;
    imgP.setId(m_imageId);
    imgP.setImageHash(imgHash);
    imgP.setTotalImageSize(m_imageBytes.size());

    uint64_t curPacketNo {};
    for (auto& p : dataPackets) {
        imgP.setPayload(std::move(p));
        imgP.setFragmentStart(ImagePacket::MTU_PAYLOAD_SIZE * curPacketNo);

        m_cachedPackets.push_back(imgP.convertToPacketPart());
        ++curPacketNo;
    }

    m_imageChanged = false;
    return m_cachedPackets;
}

} // namespace Protocol
