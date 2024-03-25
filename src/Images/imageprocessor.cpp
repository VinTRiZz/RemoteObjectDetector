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
#include "opencvimage.hpp"

struct Analyse::Processor::AnalysatorPrivate
{
    std::vector<Analyse::ImageObject> m_types; // Contain types listed in config file in the same dir with neural nets
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

std::pair<std::string, float> Analyse::Processor::getObjects(const std::string &imageFilePath, float minimalMatch, bool easyMode)
{
    std::map<std::string, float> matches;
    std::vector<std::shared_ptr<std::thread>> processThreads; // Separate computing

    const uint16_t coreCount = std::thread::hardware_concurrency();
    uint16_t currentCore {0};

    std::mutex addMutex;

    for (auto& t : d->m_types)
    {
        if (coreCount > currentCore)
        {
            processThreads.push_back(
                std::shared_ptr<std::thread>(
                    new std::thread([&](){
                        float tempMatchPercent {0};
                        tempMatchPercent = t.templateMatch(imageFilePath, templatePath);
                        addMutex.lock();
                        matches[t.name] = tempMatchPercent;
                        addMutex.unlock();
                    }),
                    [](std::thread * pThread)
                    {
                        if (pThread->joinable())
                            pThread->join();
                        delete pThread;
                    }
                )
            );

            currentCore++;
        }
        else
        {
            processThreads.begin()->reset();
            processThreads.erase(processThreads.begin(), processThreads.begin() + 1);

            processThreads.push_back(
                std::shared_ptr<std::thread>(
                    new std::thread([&](){
                        float tempMatchPercent {0};
                        tempMatchPercent = t.match(imageFilePath, minimalMatch, easyMode);
                        addMutex.lock();
                        matches[t.name] = tempMatchPercent;
                        addMutex.unlock();
                    }),
                    [](std::thread * pThread)
                    {
                        if (pThread->joinable())
                            pThread->join();
                        delete pThread;
                    }
                )
            );
        }
    }

    processThreads.clear(); // Wait for all threads

    float mustBe = 0;
    std::pair<std::string, float> foundObject {"Nothing", 0};
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
    Analyse::ImageObject typ;
    typ.name = type;

    // Check if type already exist
    auto exist = std::binary_search(d->m_types.begin(), d->m_types.end(), typ, [](auto& t_a, auto& t_b){ return t_a.name < t_b.name;});
    if (exist) return;

    // Add if not exist
    d->m_types.push_back(typ);

    // Sort to detect faster
    std::sort(d->m_types.begin(), d->m_types.end(), [](auto& t_a, auto& t_b){ return t_a.name < t_b.name;});
}

bool Analyse::Processor::removeType(const std::string& type)
{
    // Check if type not exist
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.name == type); });
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
        output[pos] = d->m_types[pos].name;
    return output;
}

void Analyse::Processor::setupType(const std::string& type, const std::string& templateFile)
{
    // Find type and setup it, if found
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.name == type); });
    if (pos != d->m_types.end())
        pos->setTemplate(templateFile);
}
