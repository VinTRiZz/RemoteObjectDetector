#ifndef UL_CONNECTION_MANAGER_H
#define UL_CONNECTION_MANAGER_H

#include <QObject>
#include <QHash>
#include <QThread>

#include "connectionworker.h"

#include <functional>
namespace Utility {

namespace Network {

class ThreadManager final : public QObject
{
    Q_OBJECT
    public:
        static ThreadManager& getInstance(std::function<Exchange::Packet (const Exchange::Packet &)>& packetProcessor, QObject* parent = 0);

        void createConnection(qintptr handler);

        int availableThreads();
        bool setThreadCount(int newCount);

        bool sendData(const QString& connectionToken, const QByteArray& data);

        void setConnectionCallbacks(const std::function<Exchange::Packet ()> &onConnectionCallback, const std::function<void (const QString &)> &onDisconnectedCallback);

    public slots:
        void onFinished();

    private:
        ThreadManager(std::function<Exchange::Packet (const Exchange::Packet &)>& packetProcessor, QObject* parent = 0);
        ~ThreadManager();

        ThreadManager(const ThreadManager&) = delete;
        ThreadManager& operator =(const ThreadManager&) = delete;
        ThreadManager& operator =(ThreadManager&&) = delete;
        ThreadManager(ThreadManager&&) = delete;

        QHash<unsigned long, std::shared_ptr<ConnectionWorker> > m_workers;
        QHash<unsigned long, std::shared_ptr<QThread> > m_threads;

        int m_maximumThreadCount {1};
        std::function<Exchange::Packet (const Exchange::Packet &)>& m_packetProcessor;
        std::function<Exchange::Packet ()> m_onConnectionCallback;
        std::function<void (const QString&)> m_onDisconnectedCallback;

        void rejectConnection(qintptr handler);
};

}

}

#endif // UL_CONNECTION_MANAGER_H
