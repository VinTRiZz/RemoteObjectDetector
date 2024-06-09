#include "packetprocessor.h"

namespace Utility {
namespace Network
{

PacketProcessor::PacketProcessor(
        std::function<Exchange::Packet (const Exchange::Packet &)> &packetProcessor,
        std::function<Exchange::Packet ()> &onConnectionCallback,
        std::function<void (const QString&)> &onDisconnectedCallback,
        QObject * parent) :
    QObject(parent),
    onConnectionCallback {onConnectionCallback},
    onDisconnectedCallback {onDisconnectedCallback},
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
