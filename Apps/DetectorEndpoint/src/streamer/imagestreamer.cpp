#include "imagestreamer.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Common/Utils.h>

#include <Components/Network/ClientUDP.h>
#include <Components/ExtraClasses/DataFragmentator.h>

struct ImageStreamer::Impl
{
    UDP::Client                     streamClient;
    ExtraClasses::DataFragmentator  imagesFragmentator;
};

ImageStreamer::ImageStreamer() :
    d {new Impl}
{

}

ImageStreamer::~ImageStreamer()
{

}

void ImageStreamer::setToken(const std::string &token)
{
    m_token = token;
}

void ImageStreamer::setHost(const std::string &serverHost, uint16_t serverPort)
{
    d->streamClient.setHost(serverHost, serverPort);
}

void ImageStreamer::sendImage(std::vector<uint8_t> &&imageData)
{
    auto imgId = Common::generateNumber(1, 10000);

    // TODO: Add token, set image number (not id), add hash

    d->imagesFragmentator.registerData(imgId, imageData.size());

    ExtraClasses::DataFragment wholeFragment;
    wholeFragment.data = std::move(imageData);
    d->imagesFragmentator.addFragment(imgId, std::move(wholeFragment));
    auto fragmentInfoPtr = d->imagesFragmentator.getData(imgId);

    const int MTU_SIZE = 1400;
    if (!fragmentInfoPtr->split(MTU_SIZE)) {
        return; // Ignore, no sense in emitting about error
    }

    for (auto& frag : fragmentInfoPtr->getFragments()) {
        d->streamClient.sendByteData(frag.data);
    }
}
