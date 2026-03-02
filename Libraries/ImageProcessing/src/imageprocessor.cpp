#include "imageprocessor.hpp"

//#include <Components/Thread/ThreadPool.h>

#include <unordered_set>
#include <list>

#include <mutex>
#include <condition_variable>

namespace {

struct DetectionInfo
{
    DetectionInfo() = default;
    DetectionInfo(DetectionInfo&& oInf) {
        id = std::move(oInf.id);
        images = std::move(oInf.images);
    }
    explicit DetectionInfo(const std::string& iId) : id {iId} {}

    std::string id {};
    std::list< std::vector<uint8_t> > images;
    std::mutex imageManipulationMx;

    bool operator ==(const DetectionInfo& oInf) const {
        return (oInf.id == id);
    }
};

}

template<>
struct std::hash<DetectionInfo> {
    std::size_t operator()(const DetectionInfo& info) const {
        return std::hash<std::string>{}(info.id);
    }
};

namespace ImageProcessing
{

struct Processor::Impl
{
    std::function<void(const std::string&, const DataObjects::DetectionObject&)> detectionCallback;
    std::unordered_set<DetectionInfo> detectionMap;

    std::condition_variable notifyCV;
    std::mutex notifyMx;

    void waitForImages() {
        std::unique_lock<std::mutex> lock(notifyMx);
        notifyCV.wait(lock);
    }

    void notifyGotImage() {
        std::unique_lock<std::mutex> lock(notifyMx);
        notifyCV.notify_one();
    }
};

Processor::Processor(unsigned int processorThreadCount) :
    d {new Impl}
{

}

Processor::~Processor()
{

}

void Processor::setImageCallback(std::function<void (const std::string &, const DataObjects::DetectionObject &)> &&cbk)
{
    d->detectionCallback = std::move(cbk);
}

void Processor::addImage(const std::string &analyseId, std::vector<uint8_t> &&imageData)
{
    auto imagesIt = d->detectionMap.find(DetectionInfo(analyseId));
    if (imagesIt == d->detectionMap.end()) {
        d->detectionMap.emplace(DetectionInfo(analyseId));
        imagesIt = d->detectionMap.find(DetectionInfo(analyseId));
    }
    auto& imageInfo = const_cast<DetectionInfo&>(*imagesIt); // Да, плохо, но я не меняю хеш, а константный метод для изменения -- странно
    imageInfo.imageManipulationMx.lock();
    imageInfo.images.emplace_back(std::move(imageData));
    imageInfo.imageManipulationMx.unlock();

    d->notifyGotImage();
}

void Processor::start()
{

}

bool Processor::isWorking() const
{
    return false;
}

void Processor::stop()
{

}

}
