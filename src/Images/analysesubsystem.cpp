#include "analysesubsystem.hpp"

#include "analysemethodmanager.hpp"
#include "cameraadaptor.hpp"
#include "typesholder.hpp"

#include <future>

struct AnalyseSubsystem::AnalyseSubsystemPrivate
{

    std::string errorText;
    bool isReady {false};
    bool isHardAnalyseEnabled {false};
    uint64_t initTimeMs {1000};
    cv::Ptr<cv::BackgroundSubtractor> m_pBackgroundSub; // Background substractor to get objects ignoring background

    TypesHolder m_typeHolder {m_pBackgroundSub};
    Adaptors::CameraAdaptor m_camera;
    Analyse::AnalyseMethodManager m_analysator{m_typeHolder};

#ifdef DEBUG_MODE
#warning "DEBUG MODE ON"
    void loadImagesForDebug(const std::string& path, std::list<cv::Mat>& targetList)
    {
        // Check if directory exist and it's directory
        if (!stdfs::exists(path) || !stdfs::is_directory(path))
        {
            std::cout << "Error: " << std::strerror(errno) << std::endl;
            return;
        }

        // Iterate in directory
        std::vector<std::string> paths;
        for (const auto& dirent : stdfs::directory_iterator(path))
        {
            // If a file, try to get image from it
            if (stdfs::is_regular_file(dirent.path()))
            {
                std::string newTypeName = dirent.path().filename();
                newTypeName.erase(newTypeName.find_last_of('.'), newTypeName.size() -1);

                // Setup type as a name of file
                paths.push_back(dirent.path().string());
            }
        }

        std::sort(paths.begin(), paths.end());

        for (auto& path : paths)
        {
            std::cout << "Loading file: " << path << std::endl;
            auto img = cv::imread(path);
            if (img.empty())
            {
                std::cout << "ERROR!" << std::endl;
                continue;
            }
            targetList.push_back(img);
        }
        return;
    }

    void testAnalyse(std::list<std::pair<std::string, double> > &analyseResultBuffer)
    {
        const std::string path = "temp/" DEBUG_DIRECTORY "/" DEBUG_TARGET_DIRECTORY;

        if (!stdfs::exists(path) || !stdfs::is_directory(path))
        {
            std::cout << "Error: no such file or not a directory: " << path << std::endl;
            return;
        }

        for (const auto& dirent : stdfs::directory_iterator(path))
        {
            if (stdfs::is_regular_file(dirent.path()))
            {
                std::cout << "Analasyng picture: " << dirent.path().filename().c_str() << std::endl;

                auto analyseTimeStart = std::chrono::high_resolution_clock::now();

                auto img = m_typeHolder.loadImage(dirent.path());
                auto result = m_analysator.detectObject(img);

                auto analyseTimeEnd = std::chrono::high_resolution_clock::now();
                auto analyseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(analyseTimeEnd - analyseTimeStart);
            }
        }
    }
#endif // DEBUG_MODE
};

AnalyseSubsystem::AnalyseSubsystem() :
    d {new AnalyseSubsystemPrivate}
{

}

AnalyseSubsystem::~AnalyseSubsystem()
{

}

void AnalyseSubsystem::setBackgroundEraseMethod(int historySize, BGR_ERASE_METHOD method)
{
    switch (method)
    {
    case BGR_ERASE_METHOD::METHOD_KNN:
        d->m_pBackgroundSub = cv::createBackgroundSubtractorKNN(historySize);   // TODO: Set count of history
        break;

    case BGR_ERASE_METHOD::METHOD_MOG2:
        d->m_pBackgroundSub = cv::createBackgroundSubtractorMOG2(historySize);   // TODO: Set count of history
        break;

    default:
        d->errorText = "Invalid analyse method";
        d->isReady = false;
        break;
    }
}

void AnalyseSubsystem::setCameraFile(const std::string &cameraDevicePath)
{
    d->m_camera.setCamera(cameraDevicePath);
    if (d->m_camera.status() == Adaptors::AdaptorStatus::READY)
    {
        d->errorText = "Invalid camera file provided";
        d->isReady = false;
    }

/*
    if (d->m_camera.status() != Adaptors::AdaptorStatus::READY)
    {
        LOG_EMPTY("========================================");
        LOG_ERROR("Camera adaptor not inited. Try to install drivers for your camera. Connected USB devices:");
        system("lsusb | sed -n 's/Bus [0-9]\\{3\\} [a-zA-Z]\\{1,\\} [0-9]\\{3\\}: ID [a-z0-9:]\\{9\\}/Device:/; /Linux Foundation/d; p'");
        LOG_EMPTY("========================================");
        return;
    }
*/
}

std::string AnalyseSubsystem::getCameraFile() const
{
    return d->m_camera.getCamera();
}

bool AnalyseSubsystem::addTemplate(const std::string &path)
{
    TypeInfoHolder tih;
    tih.typeName = stdfs::path(path).filename();
    tih.typeName.erase(tih.typeName.find_last_of('.'), tih.typeName.size());
    tih.imagePath = path;
    return d->m_typeHolder.addType(tih);
}

bool AnalyseSubsystem::renameTemplate(const std::string &oldName, const std::string &newName)
{
    for (auto& tih : d->m_typeHolder.typeList)
    {
        if (tih.typeName == oldName)
        {
            tih.typeName = newName;
            return true;
        }
    }
    return false;
}

void AnalyseSubsystem::removeTemplate(const std::string &typeName)
{
    d->m_typeHolder.typeList.erase(std::remove_if(d->m_typeHolder.typeList.begin(), d->m_typeHolder.typeList.end(), [&typeName](auto& tih){ return (tih.typeName == typeName); }));
}

void AnalyseSubsystem::setInitTime(uint64_t newTime)
{
    d->initTimeMs = newTime;
}

uint64_t AnalyseSubsystem::getInitTime() const
{
    return d->initTimeMs;
}

void AnalyseSubsystem::enableHardAnalyse(bool isEnabled)
{
    d->isHardAnalyseEnabled = isEnabled;
}

void AnalyseSubsystem::init()
{
#ifndef DEBUG_MODE
    if (!d->isReady) return;
#endif // DEBUG_MODE

    studyBackground();
}

bool AnalyseSubsystem::isReady() const
{
    return d->isReady;
}

bool AnalyseSubsystem::startAnalyse(std::list<std::pair<std::string, double> > &analyseResultBuffer)
{
#ifdef DEBUG_MODE
#warning "DEBUG MODE ON"
    d->testAnalyse(analyseResultBuffer);
    return true;
#endif // DEBUG_MODE

    // TODO: Write normally

    d->errorText = "No analyse written";
    return false;
}

void AnalyseSubsystem::pollAnalyse()
{
    // TODO: Poll
}

std::string AnalyseSubsystem::errorText() const
{
    return d->errorText;
}

void AnalyseSubsystem::studyBackground()
{
    constexpr uint64_t cameraShotPeriod = 500;

    cv::Mat tempImage;
    std::list<cv::Mat> backgrounds;

#ifdef DEBUG_MODE
#warning "DEBUG MODE ON"
    const std::string path = "./temp/" DEBUG_DIRECTORY "/background";
    d->loadImagesForDebug(path, backgrounds);
#endif // DEBUG_MODE

    for (uint64_t elapsedMs = 0; elapsedMs < d->initTimeMs; elapsedMs+= cameraShotPeriod)
    {
#ifndef DEBUG_MODE
        if (!d->m_camera.shotToBuffer(tempImage)) continue;
        backgrounds.push_back(tempImage);
#endif // DEBUG_MODE
    }

    cv::Mat result;
    for (auto& backgrd : backgrounds)
        d->m_pBackgroundSub->apply(backgrd, result);
}
