#ifndef UL_PACKETPROCESSOR_H
#define UL_PACKETPROCESSOR_H

#include <memory>
#include <functional>
#include <QObject>

#include "exchangepacket.h"

namespace Utility {
namespace Network {

class PacketProcessor : public QObject
{
    Q_OBJECT

public:
    explicit PacketProcessor(
            std::function<Exchange::Packet (const Exchange::Packet &, const QString&)>& packetProcessor,
            std::function<Exchange::Packet()>& onConnectionCallback,
            std::function<void(const QString&)>& onDisconnectedCallback,
            QObject * parent = 0);
    ~PacketProcessor();

    // Callbacks for setup and deinit
    std::function<Exchange::Packet ()>& onConnectionCallback;
    std::function<void (const QString&)>& onDisconnectedCallback;

    Exchange::Packet process(const Exchange::Packet& p, const QString&token);

private:

    // For packet processing
    std::function<Exchange::Packet (const Exchange::Packet &, const QString&)>& m_packetProcessor;
};

}

}

#endif // UL_PACKETPROCESSOR_H
