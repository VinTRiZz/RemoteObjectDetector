#include "detector.hpp"

#include "server.hpp"

namespace Web {

struct Detector::Impl
{
    DataObjects::DetectorConfiguration detectorConfig;
};

Detector::Detector(Server *parent)
    : QObject{parent},
    d {new Impl()}
{

}

Detector::~Detector()
{
    // TODO: Disconnect if need
}

const DataObjects::DetectorConfiguration &Detector::getConfiguration() const
{
    return d->detectorConfig;
}

bool Detector::setConfiguration(const DataObjects::DetectorConfiguration &conf)
{
    d->detectorConfig = conf;

    // TODO: Try add config on server

    return true;
}

} // namespace Web
