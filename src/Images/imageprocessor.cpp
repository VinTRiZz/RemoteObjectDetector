#include "imageprocessor.hpp"

// All includes for image working
#include "common.hpp"
#include "analysemethodmanager.hpp"

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
    std::list<Common::TypeInfoHolder> m_types;    // Contain all types available
    AnalyseMethodManager m_analyseManager;          // Used to compare images

    double processType(const Common::TypeInfoHolder& templateType, const cv::Mat& foundObject, Common::CompareMethod compareMethod)
    {
        // Temporary variable to save result of compare without blocking thread
        float tempMatchPercent {0};

        // Compare
        double compareResult = 0;
        switch (compareMethod)
        {
        case Common::CompareMethod::COMPARE_METHOD_TEMPLATE:
            compareResult = m_analyseManager.compareTemplate(templateType, foundObject);
            break;

        case Common::CompareMethod::COMPARE_METHOD_HISTOGRAM:
            compareResult = m_analyseManager.compareHistogram(templateType, foundObject);
            break;

        case Common::CompareMethod::COMPARE_METHOD_MOMENTS:
            compareResult = m_analyseManager.compareMoments(templateType, foundObject);
            break;
        }

        return compareResult;
    }
};

Processor::Processor() :
    d {new AnalysatorPrivate()}
{

}

Processor::~Processor()
{

}

void Processor::setImageTemplateDir(const std::string& path)
{
    d->m_types.clear();
    Common::loadObjects(path, d->m_types);
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
    std::map<std::string, float> matches;
    std::mutex matchAddMutex; // Mutex for adding match results

    // Get objects on an image
    auto targetImage = Common::loadImage(imageFilePath);
    if (targetImage.empty())
    {
        LOG_ERROR("Error loading image");
        return {};
    }
    auto objectsFound = Common::getObjects(targetImage);

    // Function to get results from threads after image processing
    auto addResult = [&](const std::string& typeName, double res){
        matchAddMutex.lock();
        matches[typeName] = res;
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
                                double tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_MOMENTS);
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
                                double tempResult = d->processType(templateType, objectOnImage, Common::CompareMethod::COMPARE_METHOD_MOMENTS);
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
    float mustBe = 0;
    std::pair<std::string, float> foundObject {"Nothing", 0};

    // Search for max match percent
    for (auto& obj : matches)
    {
        if (obj.second > mustBe)
        {
            mustBe = obj.second;
            foundObject = obj;
        }
    }

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
