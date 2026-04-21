#include "detectorserver.hpp"

#include <QEventLoop>
#include <QTimer>

#include <Components/Logger/Logger.h>

#include "implementation/detectorinfomanager.hpp"
#include "common/serverconfiguration.hpp"
#include "detector.hpp"

namespace Web {

using namespace Implementation;

struct DetectorServer::Impl
{
    bool                isOperationPending {false}; // For info downloading
    DetectorInfoManager detectorInfoInterface;
    std::vector<DetectorHandler>   detectors;
};

DetectorServer::DetectorServer(int64_t serverId, ServerRegistry *parent)
    : Server{serverId, parent},
    d {new Impl()}
{

}

DetectorServer::~DetectorServer()
{

}

bool DetectorServer::addDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto pDetector = new Detector(this);
    pDetector->setConfiguration(conf);

    // TODO: Register detector in remote

    DetectorHandler det(pDetector);
    d->detectors.push_back(det);
    std::sort(d->detectors.begin(), d->detectors.end());
    emit detectorAdded(det);
    return true;
}

void DetectorServer::removeDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto targetIt = std::find_if(d->detectors.begin(), d->detectors.end(), [&conf](const auto& detectorHdl){
        return (detectorHdl->getConfiguration() == conf);
    });
    if (targetIt == d->detectors.end()) {
        return;
    }

    // TODO: Remove detector in remote

    auto detHdl = *targetIt;
    emit detectorAboutToRemove(detHdl);
    d->detectors.erase(targetIt);

    // Delete safely
    auto pDet = detHdl.get();
    detHdl.invalidate();
    pDet->deleteLater();
}

std::vector<DetectorHandler> DetectorServer::getDetectors() const
{
    QEventLoop loop;

    connect(&d->detectorInfoInterface, &DetectorInfoManager::responseDetectorInfoList,
            &loop, [&loop, this](bool isOk, const auto& infos) {
                for (auto& info : infos) {
                    auto targetIt = std::find_if(d->detectors.begin(), d->detectors.end(), [&info](const auto& detectorHdl){
                        return (detectorHdl->getConfiguration() == info);
                    });
                    if (targetIt != d->detectors.end()) {
                        auto det = *targetIt;
                        det->setConfiguration(info);
                        std::sort(d->detectors.begin(), d->detectors.end());
                        continue;
                    }
                    auto pDetector = new Detector(const_cast<DetectorServer*>(this)); // wtf? const is not acceptable.
                    pDetector->setConfiguration(info);
                    DetectorHandler det(pDetector);
                    d->detectors.push_back(det);
                    std::sort(d->detectors.begin(), d->detectors.end());
                }
                loop.quit();
            });

    // Request timeout
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);

    d->detectorInfoInterface.requestDetectorInfoList();

    loop.exec();
    return d->detectors;
}

void DetectorServer::updateServerAddress()
{
    auto& conf = getConfiguration();
    auto serverAddr = conf.getHost() + ":" + QString::number(conf.getPort());
    d->detectorInfoInterface.setServer(serverAddr);
    Server::updateServerAddress();
}

} // namespace Web
