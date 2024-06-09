#include "imagemanipulationinterface.hpp"

#include "common.hpp"
#include "analysesubsystem.hpp"

struct ImageManipulationInterface::ImageManipulationInterfacePrivate
{
    bool isReady {false};
    std::string errorText {"success"};

    uint64_t initTimeMs {1000};
    std::list<ObjectType> types;
    std::list<std::pair<std::string, double>> tempDetectionResult;

    AnalyseSubsystem m_analysator;
};


ImageManipulationInterface::ImageManipulationInterface() :
    d {new ImageManipulationInterfacePrivate}
{

}

ImageManipulationInterface::~ImageManipulationInterface()
{

}

void ImageManipulationInterface::init()
{
    d->m_analysator.setBackgroundEraseMethod();
    d->m_analysator.init();
    d->isReady = d->m_analysator.isReady();
}

bool ImageManipulationInterface::isReady() const
{
    return d->isReady;
}

void ImageManipulationInterface::setInitTime(uint64_t newInitTime)
{
    d->m_analysator.setInitTime(newInitTime);
}

uint64_t ImageManipulationInterface::getInitTime() const
{
    return d->m_analysator.getInitTime();
}

void ImageManipulationInterface::setCamera(const std::string &cameraFile)
{
    d->m_analysator.setCameraFile(cameraFile);
}

std::string ImageManipulationInterface::getCamera() const
{
    return d->m_analysator.getCameraFile();
}

bool ImageManipulationInterface::startDetectObjects()
{
    return d->m_analysator.startAnalyse(d->tempDetectionResult);
}

void ImageManipulationInterface::pollDetecting()
{
    d->m_analysator.pollAnalyse();
}

std::list<DetectedObject> ImageManipulationInterface::detectedObjects()
{
    std::list<DetectedObject> result(d->tempDetectionResult.size());

    std::transform(d->tempDetectionResult.begin(), d->tempDetectionResult.end(), result.begin(),
    [](auto& detectionPair)
    {
        DetectedObject result;
        result.name = detectionPair.first;
        result.percent = detectionPair.second;
        return result;
    });

    return result;
}

std::string ImageManipulationInterface::lastErrorText() const
{
    return d->errorText;
}

void ImageManipulationInterface::processTemplatesDirectory(const std::string &templatesDirectoryPath)
{
    if (!stdfs::exists(templatesDirectoryPath) || !stdfs::is_directory(templatesDirectoryPath))
    {
        std::cout << "Error: no such file or not a directory: " << templatesDirectoryPath << std::endl;
        return;
    }

    for (const auto& dirent : stdfs::directory_iterator(templatesDirectoryPath))
    {
        if (stdfs::is_regular_file(dirent.path()))
        {
            std::cout << "Adding template: " << dirent.path().filename() << std::endl;
            d->m_analysator.addTemplate(dirent.path());
        }
    }
}


bool ImageManipulationInterface::addType(const ObjectType& _ot)
{
    bool result {false};
    result = d->m_analysator.addTemplate(_ot.imagePath);
    if (!result) d->errorText = d->m_analysator.errorText();
    else d->types.push_back(_ot);
    return result;
}

void ImageManipulationInterface::removeType(const std::string &_otName)
{
    d->m_analysator.removeTemplate(_otName);
}

bool ImageManipulationInterface::renameType(const std::string &_otOldName, const std::string &_otNewName)
{
    bool result {false};
    result = d->m_analysator.renameTemplate(_otOldName, _otNewName);
    if (result)
        std::find_if(d->types.begin(), d->types.end(), [&_otOldName](auto& _ot){ return (_ot.name == _otOldName); })->name = _otNewName;
    else
        d->errorText = d->m_analysator.errorText();
    return result;
}

std::list<ObjectType> ImageManipulationInterface::availableTypes() const
{
    return d->types;
}

size_t ImageManipulationInterface::availableTypesCount() const
{
    return d->types.size();
}
