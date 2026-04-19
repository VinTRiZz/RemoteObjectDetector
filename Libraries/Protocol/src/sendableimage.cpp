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
    return (m_imageId != 0);
}

void SendableImage::setSenderId(uint64_t sId)
{
    m_senderId = sId;
}

uint64_t SendableImage::getSenderId() const
{
    return m_senderId;
}

uint64_t SendableImage::getId() const
{
    return m_imageId;
}

void SendableImage::setImage(uint64_t imageId, ImageData_t &&imgData)
{
    m_imageId = imageId;
    m_imageBytes = std::move(imgData);
    m_imageChanged = true;
}

bool SendableImage::canInitFrom(const std::set<ImagePacket> &iPackets) const
{
    uint64_t imgId {};
    uint64_t senderId {};
    uint64_t prevPacketEndbyte {};
    for (auto& rp : iPackets) {
        if (prevPacketEndbyte != rp.getFragmentStart()) {
            // Invalid packet no
            return false;
        }
        auto& rpPayload = rp.getPayload();
        prevPacketEndbyte = rp.getFragmentStart() + rpPayload.size();

        if (imgId == 0) {
            imgId = rp.getId();
            senderId = rp.getSenderId();
        } else if (imgId != rp.getId() || senderId != rp.getSenderId()) { // Invalid part of image
            return false;
        }
    }
    return true;
}

bool SendableImage::initFromPackets(std::set<ImagePacket> &&iPackets)
{
    m_imageId = {};
    m_senderId = {};
    if (iPackets.empty()) {
        m_lastErrorText = "Empty input";
        return false;
    }

    // Check packets
    uint64_t imgId {};
    uint64_t senderId {};
    uint64_t prevPacketEndbyte {};
    for (auto& rp : iPackets) {
        if (prevPacketEndbyte != rp.getFragmentStart()) {
            // Invalid packet no
            m_lastErrorText = "Invalid packet fragment start";
            return false;
        }
        auto& rpPayload = rp.getPayload();
        prevPacketEndbyte = rp.getFragmentStart() + rpPayload.size();

        if (imgId == 0) {
            imgId = rp.getId();
            senderId = rp.getSenderId();
        } else if (imgId != rp.getId() || senderId != rp.getSenderId()) { // Invalid part of image
            m_lastErrorText = "Invalid id of image or sender id";
            return false;
        }
    }

    // Init from packets data
    m_imageBytes.reserve(iPackets.begin()->getTotalImageSize());
    for (auto& rp : iPackets) {
        auto& rpPayload = rp.getPayload();
        std::copy(rpPayload.begin(), rpPayload.end(), std::back_inserter(m_imageBytes));
    }
    m_imageId = imgId;
    m_senderId = senderId;

    m_imageChanged = true;
    return true;
}

std::string_view SendableImage::getLastErrorText() const
{
    return m_lastErrorText;
}

ImageData_t &SendableImage::getImage()
{
    return m_imageBytes;
}

bool SendableImage::initFromPackets(std::vector<ImageData_t > &&iPackets)
{
    m_imageId = {};
    m_senderId = {};

    std::set<ImagePacket> readPackets;
    for (auto& ip : iPackets) {
        ImagePacket imgPart;
        if (!imgPart.initFromPacketPart(ip)) {
            m_lastErrorText = "Failed to init image part packet from raw bytes";
            return false;
        }
        readPackets.emplace(std::move(imgPart));
    }
    return initFromPackets(std::move(readPackets));
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
    imgP.setSenderId(m_senderId);
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
