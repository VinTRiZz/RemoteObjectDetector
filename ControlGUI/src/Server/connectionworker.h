#ifndef UL_CONNECTIONWORKER_H
#define UL_CONNECTIONWORKER_H

#include <QObject>
#include <QTcpSocket>

#include "packetprocessor.h"

namespace Utility
{

namespace Network
{
    class ConnectionWorker : public QObject
    {
        Q_OBJECT
        public:
            explicit ConnectionWorker(
                std::function<Exchange::Packet (const Exchange::Packet &)>& packetProcessor,
                std::function<Exchange::Packet()>& onConnectionCallback,
                std::function<void(const QString&)>& onDisconnectedCallback,
                QObject* parent = 0);
            ~ConnectionWorker();

            bool sendData(const QByteArray& dataString);

            QString getToken() const;
            unsigned long getId() const;

        public slots:
            void setConnection(int descriptor, unsigned long workerId = 0);

        signals:
            void finished();

        private:
            void onMessage();
            void onDisconnect();

            QTcpSocket* m_pCon;
            unsigned long m_workerId {0};
            PacketProcessor m_processor;

            uint64_t m_timeout {2000};
            QString m_token;
            Exchange::Packet request;
            Exchange::Packet response;
            QByteArray responsePacket;
    };
}

}
#endif // UL_CONNECTIONWORKER_H
