#include "imageprocessor.hpp"

// All includes for image working
#include "common.hpp"
#include "analysemethodmanager.hpp"

#include "cameraadaptor.hpp"

namespace Analyse
{

// Deleter for pointer to a thread, used in std::shared_ptr
auto threadDeleteFunction =
    [](std::thread * pThread)
    {
        if (pThread->joinable())
            pThread->join();
        delete pThread;
    }
;

struct Processor::AnalysatorPrivate
{
    std::list<Common::TypeInfoHolder> m_types;          // Contain all types available
    AnalyseMethodManager m_analyseManager;              // Used to compare images
    cv::Ptr<cv::BackgroundSubtractor> m_pBackgroundSub; // Background substractor to get objects ignoring background
    Adaptors::CameraAdaptor m_camera;

    double processType(const Common::TypeInfoHolder& templateType, const cv::Mat& foundObject, Common::CompareMethod compareMethod)
    {
        // Temporary variable to save result of compare without blocking thread
        float tempMatchPercent {0};

        // Compare
        double compareResult = 0;
        switch (compareMethod)
        {
        case Common::CompareMethod::COMPARE_METHOD_TEST:
            compareResult = m_analyseManager.compareTest(templateType, foundObject);
            break;

        case Common::CompareMethod::COMPARE_METHOD_TEMPLATE:
            compareResult = m_analyseManager.compareTemplate(templateType, foundObject);
            break;

        case Common::CompareMethod::COMPARE_METHOD_HISTOGRAM:
            compareResult = m_analyseManager.compareHistogram(templateType, foundObject);
            break;

        case Common::CompareMethod::COMPARE_METHOD_MOMENTS:
            compareResult = m_analyseManager.compareMoments(templateType, foundObject);
            break;

        default:
            return 0;
        }

        return compareResult;
    }
};

Processor::Processor() :
    d {new AnalysatorPrivate()}
{
    d->m_pBackgroundSub = cv::createBackgroundSubtractorMOG2(0);   // TODO: Set count of history
//    d->m_pBackgroundSub = cv::createBackgroundSubtractorKNN(0);  // TODO: Set count of history
}

Processor::~Processor()
{

}

void Processor::startCamera(const std::string &cameraDevicePath)
{
    d->m_camera.setCamera(cameraDevicePath);

    // Check if camera has been inited
    if (d->m_camera.status() != Adaptors::AdaptorStatus::READY)
    {
        LOG_EMPTY("========================================");
        LOG_ERROR("Camera adaptor not inited. Try to install drivers for your camera. Connected USB devices:");
        system("lsusb | sed -n 's/Bus [0-9]\\{3\\} [a-zA-Z]\\{1,\\} [0-9]\\{3\\}: ID [a-z0-9:]\\{9\\}/Device:/; /Linux Foundation/d; p'");
        LOG_EMPTY("========================================");
        return;
    }
}

void Processor::studyBackground(uint64_t timeMs)
{
    // TODO: remove, debug needs
    std::list<cv::Mat> backgrounds;
//    const std::string path = "./temp/photos1/background";
    const std::string path = "./temp/photos2/background";
//    const std::string path = "./temp/chess/background";

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
        backgrounds.push_back(img);
    }
    cv::Mat result;
    LOG_DEBUG("Found background images: %i", backgrounds.size());
    for (auto& backgrd : backgrounds) d->m_pBackgroundSub->apply(backgrd, result);
    d->m_pBackgroundSub->getBackgroundImage(result);
    cv::imwrite("bgrnd.jpg", result);
    return;


    cv::Mat bufferImage, backgroundMask;

    const uint64_t shotPeriodMs = 50;
    for (uint64_t timeElapsedMs = 0; timeElapsedMs < timeMs; timeElapsedMs+= shotPeriodMs)
    {
        if (!d->m_camera.shotToBuffer(bufferImage))
            break;
        d->m_pBackgroundSub->apply(bufferImage, backgroundMask);
        std::this_thread::sleep_for(std::chrono::milliseconds(shotPeriodMs));
    }
}

void Processor::setImageTemplateDir(const std::string& path)
{
    d->m_types.clear();
    Common::loadObjects(path, d->m_types, d->m_pBackgroundSub);

    for (auto& typ : d->m_types)
    {
        cv::imwrite(typ.typeName + ".jpg", typ.image);
    }
}


std::pair<std::string, float> Processor::getObject(const std::string &imageFilePath)
{
    if (!d->m_types.size())
    {
        LOG_ERROR("No templates provided, analyse not started");
        return {};
    }

    // Process speed increasing
    std::vector<std::shared_ptr<std::thread>> processThreads;
    const uint16_t coreCount = std::thread::hardware_concurrency();

    // Match percent for types
    std::vector<std::pair<std::string, float>> matches;
    std::mutex matchAddMutex; // Mutex for adding match results

    // Get objects on an image
    cv::Mat targetImage = Common::loadImage(imageFilePath);
    if (targetImage.empty())
    {
        LOG_ERROR("Error loading image");
        return {};
    }
    auto objectsFound = Common::getObjects(targetImage, d->m_pBackgroundSub);

    // Function to get results from threads after image processing
    auto addResult = [&](const std::string& typeName, double res){
        matchAddMutex.lock();
        matches.push_back({typeName, res});
        matchAddMutex.unlock();
    };

    // Setup thread vector
    processThreads.resize(coreCount);
    std::fill(processThreads.begin(), processThreads.end(), std::shared_ptr<std::thread>());

    // Process types
    for (auto& objectOnImage : objectsFound)
    {
        // Check for type
        for (auto& templateType : d->m_types)
        {
            // Insert processing into threads
            for (uint16_t i = 0; i < coreCount; i++)
            {
                // Add thread if any free exist
                if (!processThreads[i].use_count())
                {
                    processThreads[i] = std::shared_ptr<std::thread>(
                        new std::thread(
                            [&]()
                            {
                                double tempResult;
                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_TEST);
                                return;
#warning "Bad choise"

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_MOMENTS);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_HISTOGRAM);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_TEMPLATE);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }
                            }
                    ), threadDeleteFunction);
                    break;
                }

                // Wait for first thread and set it after join
                if (i == coreCount - 1)
                {
                    i = std::rand() % coreCount;
                    processThreads[i] = std::shared_ptr<std::thread>(
                        new std::thread(
                            [&]()
                            {
                                double tempResult;
                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_TEST);
                                return;
#warning "Bad choise"

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_MOMENTS);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_HISTOGRAM);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }

                                tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_TEMPLATE);
                                if (tempResult >= 0.9)
                                {
                                    addResult(templateType.typeName, tempResult);
                                    return;
                                }
                            }
                    ), threadDeleteFunction);
                    break;
                }
            }
        }
    }
    processThreads.clear(); // Wait for all threads to end

    // Variables to contain result of compare
    std::pair<std::string, float> foundObject {"Nothing", 0};
    if (!matches.size())
        return foundObject;

    std::sort(matches.begin(), matches.end(), [](auto& val1, auto& val2){ return (val2 > val1); });
    foundObject = matches[matches.size() - 1];
    return foundObject;
}

bool Processor::removeType(const std::string& type)
{
    // Check if type not exist
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.typeName == type); });
    if (pos == d->m_types.end())
        return false;

    // Remove type if found
    d->m_types.erase(pos);
    return true;
}

std::vector<std::string> Processor::availableTypes() const
{
    // Copy type names from internal vector
    std::vector<std::string> output;
    output.resize(d->m_types.size());
    size_t pos {0};
    std::transform(d->m_types.begin(), d->m_types.end(), output.begin(), [](auto& typeIHolder){ return typeIHolder.typeName; });
    return output;
}

}
