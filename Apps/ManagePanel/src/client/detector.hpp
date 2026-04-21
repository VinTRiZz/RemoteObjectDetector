#pragma once

#include <QObject>

#include <ROD/DetectorConfiguration.h>

#include <memory>

namespace Web {

class Server;

class Detector : public QObject
{
    Q_OBJECT
public:
    explicit Detector(Server *parent = nullptr);
    ~Detector();

    const DataObjects::DetectorConfiguration& getConfiguration() const;

signals:
    void configurationChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    friend class Server;
    void setConfiguration(const DataObjects::DetectorConfiguration& conf);
};

} // namespace Web
