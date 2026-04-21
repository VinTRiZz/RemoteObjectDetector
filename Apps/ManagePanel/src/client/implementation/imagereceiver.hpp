#pragma once

#include <QObject>
#include <QImage>

#include <memory>

namespace Web::Implementation
{

/**
 * @brief The ImageReceiver class Class for images receiving
 */
class ImageReceiver : public QObject
{
    Q_OBJECT
public:
    explicit ImageReceiver(QObject *parent = nullptr);
    ~ImageReceiver();

    void startListen(uint16_t port);
    bool isListening() const;
    void stopListen();

signals:
    void imageReceived(const QImage& img);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

}