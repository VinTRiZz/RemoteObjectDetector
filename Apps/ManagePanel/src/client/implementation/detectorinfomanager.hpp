#pragma once

#include "httpclientbase.hpp"

#include <ROD/Types.h>
#include <ROD/DetectorConfiguration.h>

class DetectorInfoManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit DetectorInfoManager(QObject *parent = nullptr);

public slots:
    void requestDetectorList();
    void requestDetectorInfo(DataObjects::id_t detectorId);

signals:
    void responseDetectorList(bool isOk, const std::vector<DataObjects::id_t>& idList);
    void responseDetectorInfo(bool isOk, const DataObjects::DetectorConfiguration& detectorConfig);
};
