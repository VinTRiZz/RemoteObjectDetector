#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

// For stable types, such as uint8_t
#include <stdint.h>

// For smart pointers
#include <memory>

// For data arrays
#include <vector>

namespace ImageAnalyse
{
    
class Processor
{
public:
    Processor();
    ~Processor();
    
    // Sets directory where processor will try to find configuration
    void setConfigDirPath(const std::string& path);
    
    // Process photo saved by path
    bool processPhoto(const std::string& imageFilePath);
    
    // Returns empty string if no image loaded and UNKNOWN if did not recognized
    std::string photoObjectType();
    
    // Object type list manipulations
    void addType(const std::string& type);
    bool removeType(const std::string& type);

    // Get vector with all types processor can detect
    std::vector<std::string> availableTypes();
    
    // Setup another type using config file (used to add types)
    // Config file is saved pre-trained deep learning neural net
    void setupType(const std::string& type, const std::string& configFile);
    
private:
    struct AnalysatorPrivate;
    std::unique_ptr<AnalysatorPrivate> d;
};
    
}

#endif // IMAGE_PROCESSOR_H
