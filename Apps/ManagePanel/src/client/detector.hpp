#pragma once

#include <QObject>

#include <memory>
#include <optional>

#include <ROD/DetectorConfiguration.h>

#include "common/commitableobject.hpp"

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
    void visibleDataChanged();

    void configurationChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    friend class DetectorServer;
    void replaceConfiguration(const DataObjects::DetectorConfiguration& conf);
    const DataObjects::DetectorConfiguration &getPendingConfiguration() const;
    void commitConfigurationUpdate();
};

} // namespace Web
