#include <gtest/gtest.h>

#include <ROD/Protocol.h>
#include <ROD/ImageProcessing/Utility.h>

// Target class
#include "imagepacket.hpp"

#include <cstring>

// Extra for comparing
template <typename T>
bool is_bitwise_equal(const T& a, const T& b) {
    return std::memcmp(&a, &b, sizeof(T)) == 0;
}


using namespace Protocol;

// Dirty, but required for debug
static auto s_enabledLogging = []() -> bool {
    ImagePacket::setLoggingEnabled(true);
    return true;
}();

ImagePacket createTestBegPacket1() {
    ImagePacket testPacket;
    testPacket.setId(123);
    testPacket.setTotalImageSize(546);
    auto imgData = ImageProcessing::Utility::generateTestImageBytes(10, 10);
    testPacket.setImageHash(ImageProcessing::Utility::calculateImageHash(imgData));
    testPacket.setPayload(std::move(imgData));
    return testPacket;
}

ImagePacket createTestBegPacket2() {
    ImagePacket testPacket;
    testPacket.setId(789);
    testPacket.setTotalImageSize(654);
    auto imgData = ImageProcessing::Utility::generateTestImageBytes(10, 10);
    testPacket.setImageHash(ImageProcessing::Utility::calculateImageHash(imgData));
    testPacket.setPayload(std::move(imgData));
    return testPacket;
}

ImagePacket createTestPacket1() {
    auto p = createTestBegPacket1();
    p.setFragmentStart(432);
    return p;
}

ImagePacket createTestPacket2() {
    auto p = createTestBegPacket2();
    p.setFragmentStart(567);
    return p;
}

TEST(ProtocolImagePacket, Serialization) {
    // Begin packets
    auto btpack = createTestBegPacket1();
    auto bserialPack = btpack.convertToPacketPart();
    ASSERT_FALSE(bserialPack.empty());

    auto btpack2 = createTestBegPacket2();
    auto bserialPack2 = btpack2.convertToPacketPart();
    ASSERT_FALSE(bserialPack2.empty());

    ASSERT_TRUE(bserialPack != bserialPack2);

    // Regular packets
    auto tpack = createTestPacket1();
    auto serialPack = tpack.convertToPacketPart();
    ASSERT_FALSE(serialPack.empty());

    auto tpack2 = createTestPacket2();
    auto serialPack2 = tpack2.convertToPacketPart();
    ASSERT_FALSE(serialPack2.empty());

    ASSERT_TRUE(serialPack != serialPack2);
}

TEST(ProtocolImagePacket, Deserialization) {
    // Begin packets
    auto btpack1 = createTestBegPacket1();
    auto bserialPack1 = btpack1.convertToPacketPart();
    ImagePacket bdeconvP1;
    ASSERT_TRUE(bdeconvP1.initFromPacketPart(bserialPack1));
    ASSERT_EQ(bdeconvP1, btpack1);

    auto btpack2 = createTestBegPacket2();
    auto bserialPack2 = btpack2.convertToPacketPart();
    ImagePacket bdeconvP2;
    ASSERT_TRUE(bdeconvP2.initFromPacketPart(bserialPack2));
    ASSERT_NE(bdeconvP2, btpack1);
    ASSERT_EQ(bdeconvP2, btpack2);

    // Middle packets
    auto tpack1 = createTestBegPacket1();
    auto serialPack1 = tpack1.convertToPacketPart();
    ImagePacket deconvP1;
    ASSERT_TRUE(deconvP1.initFromPacketPart(serialPack1));
    ASSERT_EQ(deconvP1, tpack1);

    auto tpack2 = createTestBegPacket2();
    auto serialPack2 = tpack2.convertToPacketPart();
    ImagePacket deconvP2;
    ASSERT_TRUE(deconvP2.initFromPacketPart(serialPack2));
    ASSERT_NE(deconvP2, tpack1);
    ASSERT_EQ(deconvP2, tpack2);
}
