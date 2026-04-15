#include <gtest/gtest.h>

#include "extra/transfertester.cpp"

#include <ROD/Protocol.h>
#include <ROD/ImageProcessing/Utility.h>


using checkpair_t = std::pair<Protocol::SendableImage, ImageProcessing::ImageData_t >;
checkpair_t genPacket(int w, int h) {
    Protocol::SendableImage res;
    auto sourceImg = ImageProcessing::Utility::generateTestImageBytes(w, h);
    auto imgCopy = sourceImg;
    res.setImage(123, std::move(imgCopy));
    return std::make_pair(res, sourceImg);
}

using ImageTester_t = TransferTester<Protocol::SendableImage, std::vector< ImageProcessing::ImageData_t > >;
ImageTester_t createTester(ImageProcessing::ImageData_t& sourceImage) {
    ImageTester_t transferTester;
    transferTester.setPacketGetter([](auto& p){ return p.convertToPackets(); });
    transferTester.setChecker([sourceImage](auto&& updatedPackets) -> bool {
        Protocol::SendableImage sImage;
        if (!sImage.initFromPackets(std::move(updatedPackets))) {
            std::cerr << "Failed to init image from packets" << std::endl;
            return false;
        }
        return sImage.getImage() == sourceImage;
    });
    return transferTester;
}

void testImage(int w, int h) {
    auto p = genPacket(w, h);
    auto transferTester = createTester(p.second);
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
    auto transferTester = createTester(p.second);
    transferTester.testRegular(p.first);
    transferTester.testDropFirst(p.first);
}

TEST(ProtocolUDP, DropMiddle) {
    auto p = genPacket(500, 500);
    auto transferTester = createTester(p.second);
    transferTester.testRegular(p.first);
    transferTester.testDropMiddle(p.first);
}

TEST(ProtocolUDP, DropLast) {
    auto p = genPacket(500, 500);
    auto transferTester = createTester(p.second);
    transferTester.testRegular(p.first);
    transferTester.testDropLast(p.first);
}
