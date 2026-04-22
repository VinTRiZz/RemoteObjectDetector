#include <gtest/gtest.h>

#include "extra/transfertester.cpp"

#include <ROD/Protocol.h>
#include <ROD/ImageProcessing/Utility.h>


using checkpair_t = std::pair<Protocol::SendableImage, ImageProcessing::ImageData_t >;
checkpair_t genPacket(int w, int h) {
    Protocol::SendableImage res;
    res.setSenderId(432623465);
    auto sourceImg = ImageProcessing::Utility::generateTestImageBytes(w, h);
    auto imgCopy = sourceImg;
    res.setImage(123, std::move(imgCopy));
    return std::make_pair(res, sourceImg);
}

using ImageTester_t = TransferTester<Protocol::SendableImage, std::vector< ImageProcessing::ImageData_t > >;
ImageTester_t createTester(ImageProcessing::ImageData_t& sourceImage, uint64_t expectedId) {
    ImageTester_t transferTester;
    transferTester.setPacketGetter([expectedId](auto& p){ return p.convertToPackets(); });
    transferTester.setChecker([sourceImage, expectedId](auto&& updatedPackets) -> bool {
        Protocol::SendableImage sImage;
        if (!sImage.initFromPackets(std::move(updatedPackets))) {
            std::cerr << "Failed to init image from packets: " << sImage.getLastErrorText() << std::endl;
            return false;
        }

        if (sImage.isValid() && sImage.getImage().empty()) {
            std::cerr << "Image like valid, but the internal image is NULL. Last error: " << sImage.getLastErrorText() << std::endl;
            return false;
        }

        return (sImage.getImage() == sourceImage) && (sImage.getSenderId() == expectedId);
    });
    return transferTester;
}

void testImage(int w, int h) {
    auto p = genPacket(w, h);
    auto transferTester = createTester(p.second, p.first.getSenderId());
    transferTester.testRegular(p.first);
}

TEST(ProtocolUDP, SmallImage) {
    testImage(10, 10); // Super small (only as egde case)
}

TEST(ProtocolUDP, AverageImage) {
    testImage(480, 560); // Regular image to process
}

TEST(ProtocolUDP, LargeImage) {
    testImage(3840, 2160); // 4k image (unbelieveble, but...)
}

TEST(ProtocolUDP, DropFirst) {
    auto p = genPacket(500, 500);
    auto transferTester = createTester(p.second, p.first.getSenderId());
    transferTester.testRegular(p.first);
    transferTester.testDropFirst(p.first);
}

TEST(ProtocolUDP, DropMiddle) {
    auto p = genPacket(500, 500);
    auto transferTester = createTester(p.second, p.first.getSenderId());
    transferTester.testRegular(p.first);
    transferTester.testDropMiddle(p.first);
}

TEST(ProtocolUDP, DropLast) {
    auto p = genPacket(500, 500);
    auto transferTester = createTester(p.second, p.first.getSenderId());
    transferTester.testRegular(p.first);
    transferTester.testDropLast(p.first);
}
