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
    
    void setConfigDirPath(const std::string & path);
    
    void init();
    
    // imageBytes must be data bytes from converted to .jpg image
    bool processPhoto(std::shared_ptr<uint8_t> imageBytes);
    
    // Returns empty string if no image loaded and UNKNOWN if did not recognized
    std::string photoObjectType();
    
    void addType(const std::string & type);
    std::vector<std::string> availableTypes();
    bool removeType(const std::string & type);
    
    void setupType(const std::string & type, const std::string & configFile);
    
private:
    struct AnalysatorPrivate;
    std::unique_ptr<AnalysatorPrivate> d;
};
    
}

#endif // IMAGE_PROCESSOR_H
