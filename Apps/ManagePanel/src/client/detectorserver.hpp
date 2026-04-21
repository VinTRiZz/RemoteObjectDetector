#pragma once

#include "server.hpp"

#include <vector>

namespace Web {

class DetectorServer : public Server
{
    Q_OBJECT
public:
    DetectorServer(int64_t serverId, ServerRegistry *parent = nullptr);
    ~DetectorServer();

    bool addDetector(const DataObjects::DetectorConfiguration& conf);
    void removeDetector(const DataObjects::DetectorConfiguration& conf);
    std::vector<DetectorHandler> getDetectors() const;

    QString getLastErrorText() const;

signals:
    void detectorAdded(const DetectorHandler& hdl);
    void detectorAboutToRemove(const DetectorHandler& hdl);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    // Server interface
protected:
    virtual void updateServerAddress() override;
};

} // namespace Web
