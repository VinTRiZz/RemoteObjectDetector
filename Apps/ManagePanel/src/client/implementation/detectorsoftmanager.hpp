#pragma once

#include "httpclientbase.hpp"

namespace Web::Implementation
{

class DetectorSoftManager : public HTTPClientBase
{
    Q_OBJECT
public:
    explicit DetectorSoftManager(QObject *parent = nullptr);

signals:
};

}