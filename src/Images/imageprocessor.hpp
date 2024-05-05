#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

// For data arrays
#include <map>
#include <vector>

namespace Analyse
{

enum ProcessorCompareMode
{
    PROCESSOR_COMPARE_MODE_MATCH,   // Work good when no distortion
    PROCESSOR_COMPARE_MODE_HIST,    // Work good with distortion and rotation
    PROCESSOR_COMPARE_MODE_CONTOUR  // Do not work yet
};
    
class Processor
{
public:
    Processor();
    ~Processor();

    // Sets directory where processor will try to find configuration
    void setImageTemplateDir(const std::string& path);
    void addTemplatesFromDir(const std::string& path);

    // Analyse photo saved by path, matchPercent is lowest percent to say that image found
    // Returns pair < object - match percent >
    std::pair<std::string, float> getObject(const std::string &imageFilePath, ProcessorCompareMode compareMode = ProcessorCompareMode::PROCESSOR_COMPARE_MODE_MATCH);
    
    // Object type list manipulations
    void addType(const std::string& type);
    bool removeType(const std::string& type);

    // Get vector with all types processor can detect
    std::vector<std::string> availableTypes() const;
    
    // Setup another type using config file (used to add types)
    // Template file is path to a file with image what to use as a template
    void setupType(const std::string& type, const std::string& templateFile);
    
private:
    struct AnalysatorPrivate;
    std::unique_ptr<AnalysatorPrivate> d;
};
    
}

#endif // IMAGE_PROCESSOR_H
