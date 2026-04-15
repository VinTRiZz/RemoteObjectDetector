#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

template <
        typename PacketHandleT,
        typename ContainerT>
class TransferTester
{
public:
    // Args -- source object and changed packets
    using CheckerFuncT = std::function<bool(ContainerT&&)>;

    using ContainerGetfuncT = std::function<ContainerT(const PacketHandleT&)>;

    void setChecker(CheckerFuncT&& iFunc) {
        m_checkFunc = std::move(iFunc);
    }

    void setPacketGetter(ContainerGetfuncT&& iFunc) {
        m_packetGetter = std::move(iFunc);
    }

    void testRegular(PacketHandleT iObject) {
        ASSERT_TRUE(m_checkFunc(m_packetGetter(iObject)));
    }

    void testDropFirst(PacketHandleT& iObject) {
        auto packets = m_packetGetter(iObject);
        packets.erase(packets.begin());
        ASSERT_TRUE(!m_checkFunc(std::move(packets)));
    }

    void testDropMiddle(PacketHandleT& iObject) {
        auto packets = m_packetGetter(iObject);
        std::mt19937 randomGen;
        std::uniform_int_distribution<std::size_t> gen(0, packets.size() / 3);
        packets.erase(packets.begin() + gen(randomGen) + packets.size() / 3);
        ASSERT_TRUE(!m_checkFunc(std::move(packets)));
    }

    void testDropLast(PacketHandleT& iObject) {
        auto packets = m_packetGetter(iObject);
        packets.erase(std::prev(packets.end()));
        ASSERT_TRUE(!m_checkFunc(std::move(packets)));
    }

private:
    CheckerFuncT m_checkFunc;
    ContainerGetfuncT m_packetGetter;
};
