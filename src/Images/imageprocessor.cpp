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
#include "imagetemplate.hpp"
#include "objectdetector.hpp"

#include "common.hpp"

struct Analyse::Processor::AnalysatorPrivate
{
    std::vector<Analyse::ImageComparator> m_types; // Contain types listed in config file in the same dir with neural nets
    Analyse::ObjectDetector m_objectDetector; // Detects objects on an image to process them later
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
    if (!stdfs::exists(path) || !stdfs::is_directory(path))
    {
        LOG_ERROR("Invalid directory: %s", path.c_str());
        return;
    }

    for (const auto& dirent : stdfs::directory_iterator(path))
    {
        if (stdfs::is_regular_file(dirent.path()))
        {
            std::string newTypeName = dirent.path().filename();
            newTypeName.erase(newTypeName.find_last_of('.'), newTypeName.size() -1);

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

    std::map<std::string, float> matches;
    std::mutex matchAddMutex; // Mutex for adding match results

    // Get objects on an image
    auto objectsFound = d->m_objectDetector.getObjects(imageFilePath);

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
            float tempMatchPercent {0};

            Analyse::ImageCompareMethod compareMethod = imageIsLarge ? Analyse::ImageCompareMethod::IMAGE_COMPARE_METHOD_HIST : Analyse::ImageCompareMethod::IMAGE_COMPARE_METHOD_TEMPLATE;

            tempMatchPercent = templateType.bestMatch(foundObject, compareMethod);

            matchAddMutex.lock();
            matches[templateType.getName()] = tempMatchPercent;
            matchAddMutex.unlock();
        }
    ;

    for (uint16_t i = 0; i < coreCount; i++)
        processThreads.push_back(std::shared_ptr<std::thread>());

    // Process types
    for (auto& templateType : d->m_types)
    {
        for (auto& objectOnImage : objectsFound)
        {
            for (uint16_t i = 0; i < coreCount; i++)
            {
                if (!processThreads[i].use_count())
                {
                    processThreads[i] = std::shared_ptr<std::thread>(new std::thread(processFunction, std::ref(templateType), std::ref(objectOnImage)), threadDeleteFunction);
                    break;
                }

                if (i == coreCount - 1)
                {
                    processThreads.begin()->reset();
                    processThreads[0] = std::shared_ptr<std::thread>(new std::thread(processFunction, std::ref(templateType), std::ref(objectOnImage)), threadDeleteFunction);
                }
            }
        }
    }
    processThreads.clear(); // Wait for all threads

    float mustBe = 0;
    std::pair<std::string, float> foundObject {"Nothing", 0};

    // Normalize values
//    if (compareMode == PROCESSOR_COMPARE_MODE_CONTOUR)
//    {
//        // Get max
//        double maxVal {0};
//        for (auto& obj : matches)
//        {
//            double equVal = 1.0 / obj.second;
//            if (equVal > maxVal)
//                maxVal = equVal;
//        }

//        // Get min
//        double minVal {maxVal};
//        for (auto& obj : matches)
//        {
//            double equVal = 1.0 / obj.second;
//            if (equVal < minVal)
//                minVal = equVal;
//        }

//        // Normalize
//        double coeff = maxVal - minVal;

//        LOG_DEBUG("Normalization coeffs: %.3f %.3f %.3f", minVal, maxVal, coeff);

//        for (auto& obj : matches)
//        {
//            double equVal = 1.0 / obj.second;
//            obj.second = (equVal - minVal) / coeff;
//            LOG_DEBUG("Res of normalize: %s - %f", obj.first.c_str(), obj.second);
//        }
//    }

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

    // Sort to detect faster
    std::sort(d->m_types.begin(), d->m_types.end(), [](auto& t_a, auto& t_b){ return t_a.getName() < t_b.getName();});
}

bool Analyse::Processor::removeType(const std::string& type)
{
    // Check if type not exist
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.getName() == type); });
    if (pos == d->m_types.end())
        return false;

    d->m_types.erase(pos);
    return true;
}

std::vector<std::string> Analyse::Processor::availableTypes() const
{
    // Copy type names from internal vector
    std::vector<std::string> output;
    output.resize(d->m_types.size());
    for (size_t pos = 0; pos < d->m_types.size(); pos++)
        output[pos] = d->m_types[pos].getName();
    return output;
}

void Analyse::Processor::setupType(const std::string& type, const std::string& templateFile)
{
    // Find type and setup it, if found
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.getName() == type); });
    if (pos != d->m_types.end())
        pos->setTemplate(templateFile);
}
