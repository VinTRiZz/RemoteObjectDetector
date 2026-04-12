#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

template <typename PacketHandleT, typename ContainerT>
class TransferTester
{
public:
    void testInvalidRegular(PacketHandleT input) {
        ASSERT_TRUE(false);
    }

    void testRegular(PacketHandleT input) {
        ASSERT_TRUE(false);
    }

    template <typename ContainerGetfuncT>
    void testDropFirst(PacketHandleT& iPacket, ContainerGetfuncT&& packetDataGetter) {
        ASSERT_TRUE(impl_testDropFirst(packetDataGetter(iPacket)));
    }

    template <typename ContainerGetfuncT>
    void testDropMiddle(PacketHandleT& iPacket, ContainerGetfuncT&& packetDataGetter) {
        ASSERT_TRUE(impl_testDropMiddle(packetDataGetter(iPacket)));
    }

    template <typename ContainerGetfuncT>
    void testDropLast(PacketHandleT& iPacket, ContainerGetfuncT&& packetDataGetter) {
        ASSERT_TRUE(impl_testDropLast(packetDataGetter(iPacket)));
    }

private:
    bool impl_testDropFirst(ContainerT&& iPacket) {
        iPacket.erase(iPacket.begin());
        return false;
    }

    bool impl_testDropMiddle(ContainerT&& iPacket) {
        std::mt19937 randomGen;
        std::uniform_int_distribution<std::size_t> gen(0, iPacket.size() / 3);
        iPacket.erase(iPacket.begin() + gen(randomGen) + iPacket.size() / 3);
        return false;
    }

    bool impl_testDropLast(ContainerT&& iPacket) {
        iPacket.erase(std::prev(iPacket.end()));
        return false;
    }
};
