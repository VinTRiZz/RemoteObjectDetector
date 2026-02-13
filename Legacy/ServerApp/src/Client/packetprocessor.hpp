#ifndef UL_PACKETPROCESSOR_H
#define UL_PACKETPROCESSOR_H

#include <memory>
#include <functional>
#include <QObject>

#include "exchangepacket.hpp"

namespace Utility {
namespace Network {

class PacketProcessor : public QObject
{
    Q_OBJECT

public:
    explicit PacketProcessor(std::function<Exchange::Packet (const Exchange::Packet &)>& packetProcessor, QObject * parent = 0);
    ~PacketProcessor();

    Exchange::Packet process(const Exchange::Packet& p);

private:

    // For packet processing
    std::function<Exchange::Packet (const Exchange::Packet &)>& m_packetProcessor;
};

}

}

#endif // UL_PACKETPROCESSOR_H
