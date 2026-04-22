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

    QString lastErrorText;

    void updateDetectorData(const DetectorHandler& hdl) {
        if (!hdl.isValid()) {
            return;
        }
        detectorInfoInterface.requestUpdateDetectorInfo(hdl->getPendingConfiguration());
    }
};

DetectorServer::DetectorServer(int64_t serverId, ServerRegistry *parent)
    : Server{serverId, parent},
    d {new Impl()}
{
    connect(&d->detectorInfoInterface, &DetectorInfoManager::responseUpdateDetectorInfo,
                  this, [this](bool isOk, auto detId){
        for (auto& det : d->detectors) {
            if (det.isValid() && det->getConfiguration().system.id == detId) {
                det->commitConfigurationUpdate();
                return;
            }
        }
    });
}

DetectorServer::~DetectorServer()
{

}

bool DetectorServer::addDetector(const DataObjects::DetectorConfiguration &conf)
{
    auto addResult = d->detectorInfoInterface.addDetectorInfo(conf);
    if (!addResult.has_value()) {
        d->lastErrorText = d->detectorInfoInterface.getLastErrorText();
        return false;
    }
    auto confCopy = conf;
    confCopy.system.id = addResult.value();

    auto pDetector = new Detector(this);
    pDetector->replaceConfiguration(confCopy);

    DetectorHandler det(pDetector);
    connect(pDetector, &Detector::configurationChanged,
            this, [this, det](){
                d->updateDetectorData(det);
            });
    d->detectors.push_back(det);
    std::sort(d->detectors.begin(), d->detectors.end());
    emit detectorAdded(det);
    return true;
}

bool DetectorServer::removeDetector(const DataObjects::id_t &detId)
{
    auto targetIt = std::find_if(d->detectors.begin(), d->detectors.end(), [detId](const auto& detectorHdl){
        return (detectorHdl->getConfiguration().system.id == detId);
    });
    if (targetIt == d->detectors.end()) {
        d->lastErrorText = "No such detector or it's just removed";
        return false;
    }

    auto removeResult = d->detectorInfoInterface.removeDetectorInfo(detId);
    if (!removeResult) {
        d->lastErrorText = d->detectorInfoInterface.getLastErrorText();
        return false;
    }

    auto detHdl = *targetIt;
    emit detectorAboutToRemove(detHdl);
    d->detectors.erase(targetIt);

    // Delete safely
    auto pDet = detHdl.get();
    detHdl.invalidate();
    pDet->deleteLater();

    return true;
}

bool DetectorServer::removeDetector(const DetectorHandler &hdl)
{
    return removeDetector(hdl->getConfiguration().system.id);
}

std::vector<DetectorHandler> DetectorServer::getDetectors() const
{
    QEventLoop loop;

    connect(&d->detectorInfoInterface, &DetectorInfoManager::responseDetectorInfoList,
            &loop, [&loop, this](bool isOk, const auto& infos) {

                for (auto& info : infos) {
                    auto targetIt = std::find_if(d->detectors.begin(), d->detectors.end(), [&info](const auto& detectorHdl){
                        return (detectorHdl->getConfiguration().system.id == info.system.id);
                    });
                    if (targetIt != d->detectors.end()) {
                        auto det = *targetIt;
                        det->blockSignals(true);
                        det->replaceConfiguration(info);
                        det->blockSignals(false);
                        std::sort(d->detectors.begin(), d->detectors.end());
                        continue;
                    }

                    auto pDetector = new Detector(const_cast<DetectorServer*>(this)); // wtf? const is not acceptable.
                    pDetector->replaceConfiguration(info);
                    DetectorHandler det(pDetector);
                    d->detectors.push_back(det);
                    std::sort(d->detectors.begin(), d->detectors.end());

                    connect(pDetector, &Detector::configurationChanged,
                            this, [this, det](){
                                d->updateDetectorData(det);
                            });
                }
                loop.quit();
            });

    // Request timeout
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);

    d->detectorInfoInterface.requestDetectorInfoList();

    loop.exec();
    return d->detectors;
}

QString DetectorServer::getLastErrorText() const
{
    return d->lastErrorText;
}

void DetectorServer::updateServerAddress()
{
    auto& conf = getConfiguration();
    auto serverAddr = conf.getHost() + ":" + QString::number(conf.getPort());
    d->detectorInfoInterface.setServer(serverAddr);
    Server::updateServerAddress();
}

} // namespace Web
