#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include <QObject>
#include <QImage>

#include "commonstructs.h"

#include "Server/tcpserverinstance.h"

class ControlServer : public QObject
{
    Q_OBJECT
public:
    explicit ControlServer(QObject *parent = nullptr);
    ~ControlServer();

    bool init(const uint16_t portNo);

    void setup(const QString& token)        { request(Exchange::PACKET_INFO_CT_SETUP, token); }
    void reconnect(const QString& token)    { request(Exchange::PACKET_INFO_CT_RECONNECT, token); }
    void reboot(const QString& token)       { request(Exchange::PACKET_INFO_CT_REBOOT, token); }
    void status(const QString& token)       { request(Exchange::PACKET_INFO_CT_STATUS, token); }

    void start(const QString& token)        { request(Exchange::PACKET_INFO_CT_START, token); }
    void stop(const QString& token)         { request(Exchange::PACKET_INFO_CT_STOP, token); }

    void photo(const QString& token)        { request(Exchange::PACKET_INFO_CT_PHOTO, token); }

    void addObject(const QString& objectName, const QString& token)                             { request(Exchange::PACKET_INFO_CT_ADD_OBJECT, token, objectName); }
    void renameObject(const QString& objectName, const QString& newName, const QString& token)  { request(Exchange::PACKET_INFO_CT_REM_OBJECT, token, objectName + "****" + newName); }
    void removeObject(const QString& objectName, const QString& token)                          { request(Exchange::PACKET_INFO_CT_REM_OBJECT, token, objectName); }

    // Get last downloaded image
    QImage getPhoto() { return m_imageIsDownloading ? m_bufferPhoto : QImage(); }

signals:
    void deviceConnected(const QString& devToken);
    void deviceDisconnected(const QString& devToken);

    void deviceSetupComplete(const QString& devToken);
    void deviceStarted(bool isStarted);
    void deviceStopped(bool isStopped);

    void objectAdded(const QString& objectName);
    void objectRenamed(const QString& objectName, const QString& newName);
    void objectRemoved(const QString& objectName);

    void deviceStatusGot(const Exchange::StatusData& devStatus);
    void photoGot();

    void errorGot(const QString& errorText);

private:
    Utility::Network::TcpServerInstanceQ* m_server;

    void request(Exchange::PacketMetaInfo commandCode, const QString& token, const QString payload = {});
    Exchange::Packet processPacket(const Exchange::Packet& request);
    Exchange::Packet onConnected();
    void onDisconnected(const QString& token);

    // For image downloading
    bool m_imageIsDownloading {false};
    QByteArray m_photoDataBuffer;
    QImage m_bufferPhoto;
};

#endif // CONTROLSERVER_H
