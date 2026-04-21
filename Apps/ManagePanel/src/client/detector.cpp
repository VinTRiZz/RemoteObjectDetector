#include "detector.hpp"

#include "server.hpp"

namespace Web {

struct Detector::Impl
{
    CommitableObject<DataObjects::DetectorConfiguration> config;
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
    d->config = conf;
    emit configurationChanged();
}

const DataObjects::DetectorConfiguration &Detector::getConfiguration() const
{
    return d->config.source();
}

bool Detector::operator<(const Detector &det) const
{
    if (d->config->info.name.empty() ||
        d->config->info.name == det.d->config->info.name) {
        return d->config->system.id < det.d->config->system.id;
    }
    return d->config->info.name < det.d->config->info.name;
}

void Detector::replaceConfiguration(const DataObjects::DetectorConfiguration &conf)
{
    d->config = conf;
    d->config.commit();
    emit visibleDataChanged();
}

CommitableObject<DataObjects::DetectorConfiguration> Detector::getPendingConfiguration() const
{
    return d->config;
}

void Detector::commitConfigurationUpdate()
{
    d->config.commit();
}

} // namespace Web
