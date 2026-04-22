#pragma once

#include "httpclientbase.hpp"

#include <ROD/Types.h>
#include <ROD/DetectorConfiguration.h>

#include <optional>

namespace Web::Implementation
{

class DetectorInfoManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit DetectorInfoManager(QObject *parent = nullptr);

    // TODO: set as Error object
    QString getLastErrorText() const;

    DataObjects::id_t addDetectorInfo(const DataObjects::DetectorConfiguration& detectorConfig);
    bool removeDetectorInfo(DataObjects::id_t detectorId);

public slots:
    void requestDetectorList();
    void requestDetectorInfoList();

    void requestGetDetectorInfo(DataObjects::id_t detectorId);
    void requestUpdateDetectorInfo(const DataObjects::DetectorConfiguration& detectorConfig);

signals:
    void responseDetectorList(bool isOk, const std::vector<DataObjects::id_t>& idList);
    void responseDetectorInfoList(bool isOk, const std::vector<DataObjects::DetectorConfiguration>& detectorConfig);

    void responseGetDetectorInfo(bool isOk, const DataObjects::DetectorConfiguration& detectorConfig);
    void responseUpdateDetectorInfo(bool isOk, const DataObjects::id_t& detectorId);

private:
    QString m_lastErrorText;
};

}