#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

// Data containers
#include <map>
#include <vector>

namespace Analyse
{

class Processor
{
public:
    Processor();
    ~Processor();

    // Sets directory where processor will try to find configuration
    void setImageTemplateDir(const std::string& path);

    // Add templates from directory if they exist
    void addTemplatesFromDir(const std::string& path);

    // Analyse image saved by path
    // Returns pair < object - match percent >
    std::pair<std::string, float> getObject(const std::string &imageFilePath);
    
    // Add type of object without describing
    void addType(const std::string& type);

    // Remove type from known. If succeed, return true
    bool removeType(const std::string& type);

    // Get vector with all types processor can detect
    std::vector<std::string> availableTypes() const;
    
    // Setup type using image by path to make type valid
    void setupType(const std::string& type, const std::string& templateFile);
    
private:
    struct AnalysatorPrivate;
    std::unique_ptr<AnalysatorPrivate> d;
};
    
}

#endif // IMAGE_PROCESSOR_H
