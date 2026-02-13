#include "packetprocessor.hpp"

namespace Utility {
namespace Network
{

PacketProcessor::PacketProcessor(std::function<Exchange::Packet (const Exchange::Packet &)> &packetProcessor, QObject * parent) :
    QObject(parent),
    m_packetProcessor {packetProcessor}
{

}

PacketProcessor::~PacketProcessor()
{

}

Exchange::Packet PacketProcessor::process(const Exchange::Packet& p)
{
    if (m_packetProcessor) return m_packetProcessor(p);
    return {};
}

}
}
