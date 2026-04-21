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

    void setConfiguration(const DataObjects::DetectorConfiguration& conf);
    const DataObjects::DetectorConfiguration& getConfiguration() const;

    bool operator<(const Detector& det) const;
signals:
    void configurationChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace Web
