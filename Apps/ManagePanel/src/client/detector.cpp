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

}

void Detector::setConfiguration(const DataObjects::DetectorConfiguration &conf)
{
    d->detectorConfig = conf;
    emit configurationChanged();
    emit visibleDataChanged();
}

const DataObjects::DetectorConfiguration &Detector::getConfiguration() const
{
    return d->detectorConfig;
}

bool Detector::operator<(const Detector &det) const
{
    if (d->detectorConfig.info.name.empty() ||
        d->detectorConfig.info.name == det.d->detectorConfig.info.name) {
        return d->detectorConfig.system.id < det.d->detectorConfig.system.id;
    }
    return d->detectorConfig.info.name < det.d->detectorConfig.info.name;
}

} // namespace Web
