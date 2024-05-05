#include "imageprocessor.hpp"

// STD algorithms
#include <algorithm>

// Terminal data output
#include <iostream>

// Directory processing
#include <dirent.h>

// Log info
#include "logging.hpp"

// Fast comparing
#include <thread>
#include <mutex>

// OpenCV image comparator
#include "imagecomparator.hpp"

#include "common.hpp"

struct Analyse::Processor::AnalysatorPrivate
{
    std::vector<Analyse::ImageComparator> m_types; // Contain all types available
};

Analyse::Processor::Processor() :
    d {new AnalysatorPrivate()}
{

}

Analyse::Processor::~Processor()
{

}

void Analyse::Processor::setImageTemplateDir(const std::string& path)
{
    d->m_types.clear();
    addTemplatesFromDir(path);
}

void Analyse::Processor::addTemplatesFromDir(const std::string &path)
{
    // Check if directory exist and it's directory
    if (!stdfs::exists(path) || !stdfs::is_directory(path))
    {
        LOG_ERROR("Invalid directory: %s", path.c_str());
        return;
    }

    // Iterate in directory
    for (const auto& dirent : stdfs::directory_iterator(path))
    {
        // If a file, try to get image from it
        if (stdfs::is_regular_file(dirent.path()))
        {
            std::string newTypeName = dirent.path().filename();
            newTypeName.erase(newTypeName.find_last_of('.'), newTypeName.size() -1);

            // Setup type as a name of file
            addType(newTypeName);
            setupType(newTypeName, dirent.path());
        }
    }
}

std::pair<std::string, float> Analyse::Processor::getObject(const std::string &imageFilePath)
{
    // Process speed increasing
    std::vector<std::shared_ptr<std::thread>> processThreads;
    const uint16_t coreCount = std::thread::hardware_concurrency();

    // Match percent for types
    std::map<std::string, float> matches;
    std::mutex matchAddMutex; // Mutex for adding match results

    // Get objects on an image
    auto objectsFound = Common::getObjects(imageFilePath);

    // Check if image is large (can contain more than one object)
    const uint64_t LARGE_IMAGE_BORDER {500 * 500};
    cv::Mat img = Common::loadImage(imageFilePath);
    bool imageIsLarge = img.rows * img.cols >= LARGE_IMAGE_BORDER;

    // Deleter for pointer to a thread, used in std::shared_ptr
    auto threadDeleteFunction =
        [](std::thread * pThread)
        {
            if (pThread->joinable())
                pThread->join();
            delete pThread;
        }
    ;

    // Used to process current image
    auto processFunction =
        [&](Analyse::ImageComparator& templateType, cv::Mat& foundObject)
        {
            // Temporary variable to save result of compare without blocking thread
            float tempMatchPercent {0};

            // Check what method to use for compare
            Analyse::ImageCompareMethod compareMethod = imageIsLarge ? Analyse::ImageCompareMethod::IMAGE_COMPARE_METHOD_HIST : Analyse::ImageCompareMethod::IMAGE_COMPARE_METHOD_TEMPLATE;

            // Compare
            tempMatchPercent = templateType.bestMatch(foundObject, compareMethod);

            // Add match to result map
            matchAddMutex.lock();
            matches[templateType.getName()] = tempMatchPercent;
            matchAddMutex.unlock();
        }
    ;

    // Setup thread vector
    for (uint16_t i = 0; i < coreCount; i++)
        processThreads.push_back(std::shared_ptr<std::thread>());

    // Process types
    for (auto& templateType : d->m_types)
    {
        // Process objects found on an image
        for (auto& objectOnImage : objectsFound)
        {
            // Insert processing into threads
            for (uint16_t i = 0; i < coreCount; i++)
            {
                // Add thread if any free exist
                if (!processThreads[i].use_count())
                {
                    processThreads[i] = std::shared_ptr<std::thread>(new std::thread(processFunction, std::ref(templateType), std::ref(objectOnImage)), threadDeleteFunction);
                    break;
                }

                // Wait for first thread and set it after join
                if (i == coreCount - 1)
                {
                    processThreads[0] = std::shared_ptr<std::thread>(new std::thread(processFunction, std::ref(templateType), std::ref(objectOnImage)), threadDeleteFunction);
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

void Analyse::Processor::addType(const std::string& type)
{
    Analyse::ImageComparator typ;
    typ.setName(type);

    // Check if type already exist
    auto exist = std::binary_search(d->m_types.begin(), d->m_types.end(), typ, [](auto& t_a, auto& t_b){ return t_a.getName() < t_b.getName();});
    if (exist) return;

    // Add if not exist
    d->m_types.push_back(typ);

    // Sort types using lexicographical comparison
    std::sort(d->m_types.begin(), d->m_types.end(), [](auto& t_a, auto& t_b){ return t_a.getName() < t_b.getName();});
}

bool Analyse::Processor::removeType(const std::string& type)
{
    // Check if type not exist
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.getName() == type); });
    if (pos == d->m_types.end())
        return false;

    // Remove type if found
    d->m_types.erase(pos);
    return true;
}

std::vector<std::string> Analyse::Processor::availableTypes() const
{
    // Copy type names from internal vector
    std::vector<std::string> output;
    output.resize(d->m_types.size());
    size_t pos {0};
    std::generate(output.begin(), output.end(), [this, &pos](){ return d->m_types[pos++].getName(); });
    return output;
}

void Analyse::Processor::setupType(const std::string& type, const std::string& templateFile)
{
    // Find type and setup it, if found
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.getName() == type); });
    if (pos != d->m_types.end())
        pos->setImage(templateFile);
}
