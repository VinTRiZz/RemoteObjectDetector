#pragma once

#include "httpclientbase.hpp"

#include <ROD/Types.h>
#include <ROD/DetectorConfiguration.h>

namespace Web::Implementation
{

class DetectorInfoManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit DetectorInfoManager(QObject *parent = nullptr);

public slots:
    void requestDetectorList();
    void requestDetectorInfo(DataObjects::id_t detectorId);
    void requestDetectorInfoList();

signals:
    void responseDetectorList(bool isOk, const std::vector<DataObjects::id_t>& idList);
    void responseDetectorInfo(bool isOk, const DataObjects::DetectorConfiguration& detectorConfig);
    void responseDetectorInfoList(bool isOk, const std::vector<DataObjects::DetectorConfiguration>& detectorConfig);
};

}