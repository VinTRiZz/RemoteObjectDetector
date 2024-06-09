#include "threadmanager.h"

#include <QTcpSocket>
#include <QDebug>

#define THREAD_SHUTDOWN_TIMEOUT_MS 1000

Utility::Network::ThreadManager::ThreadManager(std::function<Exchange::Packet (const Exchange::Packet &, const QString&)>& packetProcessor, QObject *parent) :
    QObject{parent},
    m_packetProcessor{ packetProcessor }
{
    setThreadCount( QThread::idealThreadCount() );
}

Utility::Network::ThreadManager::~ThreadManager()
{

}

Utility::Network::ThreadManager &Utility::Network::ThreadManager::getInstance(std::function<Exchange::Packet (const Exchange::Packet &, const QString&)> &packetProcessor, QObject* parent)
{
    static ThreadManager manager(packetProcessor, parent);
    return manager;
}

void Utility::Network::ThreadManager::rejectConnection(qintptr handler)
{
    // Reject connection if no threads available
    QTcpSocket socket;
    socket.setSocketDescriptor(handler);
    socket.disconnect();
}


void Utility::Network::ThreadManager::createConnection(qintptr handler)
{
    if (m_workers.size() >= m_maximumThreadCount)
    {
        qDebug() << "[\033[31mE\033[0m] No threads available";
        return rejectConnection(handler); // Can't create thread, so can't work with connection
    }

    // Setup ID of worker
    unsigned long workerId = std::rand() * std::rand();
    while (m_workers.find(workerId) != m_workers.end())
        workerId = std::rand() * std::rand();

    // Remember worker and it's thread
    m_workers[workerId] =
            std::shared_ptr<ConnectionWorker>(
                new ConnectionWorker(m_packetProcessor, m_onConnectionCallback, m_onDisconnectedCallback),
                [this](ConnectionWorker* pWorker)
                {
                    disconnect(pWorker, &ConnectionWorker::finished, this, &ThreadManager::onFinished);
                }
    );

    m_threads[workerId] =
            std::shared_ptr<QThread>(
                new QThread(this),
                [](QThread* pThread)
                {
                    pThread->exit();
                    pThread->wait(THREAD_SHUTDOWN_TIMEOUT_MS);
                }
    );

    auto& pWorker = m_workers[workerId];
    auto& pThread = m_threads[workerId];

    connect(pWorker.get(), &ConnectionWorker::finished, this, &ThreadManager::onFinished);

    // Setup worker
    pWorker->moveToThread(pThread.get());
    pThread->start();
    QMetaObject::invokeMethod(pWorker.get(), "setConnection", Qt::QueuedConnection, Q_ARG(int, handler), Q_ARG(unsigned long, workerId));
}

int Utility::Network::ThreadManager::availableThreads()
{
    return (m_maximumThreadCount - m_workers.size());
}

bool Utility::Network::ThreadManager::setThreadCount(int newCount)
{
    if ((m_workers.size() > 0) || (newCount < 0))
        return false;
    m_maximumThreadCount = newCount;
    return true;
}

bool Utility::Network::ThreadManager::hasConnection(const QString token)
{
    qDebug() << "TM Requesting if exist" << token;
    for (auto& worker : m_workers)
    {
        if (token == worker->getToken())
        {
            qDebug() << "Exist";
            return true;
        }
    }
    qDebug() << "Not found";
    return false;
}

bool Utility::Network::ThreadManager::sendData(const QString &connectionToken, const QByteArray &data)
{
    auto pConWorker = std::find_if(m_workers.begin(), m_workers.end(), [&connectionToken](auto& con){ return (con->getToken() == connectionToken); });
    if (pConWorker == m_workers.end())
        return false;

    QMetaObject::invokeMethod(pConWorker->get(), "sendData", Qt::QueuedConnection, Q_ARG(const QByteArray&, data));
    return true;
}

void Utility::Network::ThreadManager::setConnectionCallbacks(const std::function<Exchange::Packet ()> &onConnectionCallback, const std::function<void (const QString&)> &onDisconnectedCallback)
{
    m_onConnectionCallback = onConnectionCallback;
    m_onDisconnectedCallback = onDisconnectedCallback;
}

void Utility::Network::ThreadManager::onFinished()
{
    ConnectionWorker * pWorker = static_cast<ConnectionWorker *>(sender());
    disconnect(pWorker, &ConnectionWorker::finished, this, &ThreadManager::onFinished);

    auto pWorkerThread = m_threads.find(pWorker->getId());
//    pWorkerThread.value()->exit();
//    pWorkerThread.value()->wait(THREAD_SHUTDOWN_TIMEOUT_MS);
    m_threads.erase(pWorkerThread);
    m_workers.erase(m_workers.find(pWorker->getId()));
}
