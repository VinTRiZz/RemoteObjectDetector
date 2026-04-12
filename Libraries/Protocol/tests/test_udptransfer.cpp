#include <gtest/gtest.h>

#include "extra/transfertester.cpp"

#include <ROD/Protocol.h>
#include <ROD/ImageProcessingUtility.h>

Protocol::SendableImage genPacket(int w, int h) {
    Protocol::SendableImage res;
    res.setImage(123, ImageProcessing::Utility::generateTestImageBytes(w, h));
    return res;
}

void testImage(int w, int h) {
    TransferTester<Protocol::SendableImage, std::vector< std::vector<uint8_t> > > transferTester;
    auto p = genPacket(w, h);
    transferTester.testRegular(p);
}

TEST(ProtocolUDP, InvalidImage) {
    TransferTester<Protocol::SendableImage, std::vector< std::vector<uint8_t> > > transferTester;
    Protocol::SendableImage p;
    transferTester.testInvalidRegular(p);
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
    TransferTester<Protocol::SendableImage, std::vector< std::vector<uint8_t> > > transferTester;
    auto p = genPacket(500, 500);
    transferTester.testRegular(p);
    transferTester.testDropFirst(p,  [](auto& p){ return p.convertToPackets(); });
}

TEST(ProtocolUDP, DropMiddle) {
    TransferTester<Protocol::SendableImage, std::vector< std::vector<uint8_t> > > transferTester;
    auto p = genPacket(500, 500);
    transferTester.testRegular(p);
    transferTester.testDropMiddle(p, [](auto& p){ return p.convertToPackets(); });
}

TEST(ProtocolUDP, DropLast) {
    TransferTester<Protocol::SendableImage, std::vector< std::vector<uint8_t> > > transferTester;
    auto p = genPacket(500, 500);
    transferTester.testRegular(p);
    transferTester.testDropLast(p,   [](auto& p){ return p.convertToPackets(); });
}