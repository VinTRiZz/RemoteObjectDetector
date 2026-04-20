#pragma once

#include "httpclientbase.hpp"

class DetectorSoftManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit DetectorSoftManager(QObject *parent = nullptr);

signals:
};
