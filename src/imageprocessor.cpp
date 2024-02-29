#include "imageprocessor.h"

// Template matching from OpenCV
#include <opencv2/opencv.hpp>

// STD algorithms
#include <algorithm>

// Terminal data output
#include <iostream>

#define PRINT_LOG(what) std::cout << "[ " << __FUNCTION__ << " ] " << what << std::endl
#define PRINT_SUC(what) std::cout << "[\033[32m OK! \033[0m] " << what << std::endl
#define PRINT_ERR(what) std::cout << "[\033[31mERROR\033[0m] [ " << __PRETTY_FUNCTION__ << " ] " << what << std::endl

namespace ImageAnalyse
{

struct ImageType
{
private:
    std::string templatePath;
    cv::Mat templateImage;

public:
    std::string name;

    ImageType() = default;
    ~ImageType() = default;

    bool operator !=(const ImageType& ot)
    {
        return (ot.name == name);
    }

    void setTemplate(const std::string& filePath)
    {
        templateImage = cv::imread(filePath);
    }

    bool isObject(const std::string& filePath, const double matchPercent = 0.8)
    {
        if (templateImage.empty())
            return false;

        cv::Mat img = cv::imread(filePath);
        cv::Mat result;

        if (img.empty())
            return false;

        // Compare to template
        cv::matchTemplate(img, templateImage, result, cv::TM_CCOEFF_NORMED); // TODO: Experiment with comp methods

        double minVal {}, maxVal {};
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        PRINT_LOG("Match percent: " << maxVal * 100.0);

        // Check if image found
        if (maxVal < matchPercent)
            return false;
        return true;
    }
};

}

struct ImageAnalyse::Processor::AnalysatorPrivate
{
    std::string m_configDirPath;    // Path to directory with saved neural nets
    std::string m_detectedType;     // Used to show what type of object on image must be

    std::vector<ImageType> m_types; // Contain types listed in config file in the same dir with neural nets

    // Used to process saved neural nets in m_configDirPath directory
    void processDirectory()
    {
        // TODO: Read all NN in the dir
    }
};

ImageAnalyse::Processor::Processor() :
    d {new AnalysatorPrivate()}
{

}

ImageAnalyse::Processor::~Processor()
{

}

void ImageAnalyse::Processor::setConfigDirPath(const std::string& path)
{
    d->m_configDirPath = path;
    d->processDirectory();  // Update contents
}

bool ImageAnalyse::Processor::processPhoto(const std::string& imageFilePath, const double matchPercent)
{
    d->m_detectedType = "";
    for (auto& t : d->m_types)
    {
        if (t.isObject(imageFilePath, matchPercent))
        {
            d->m_detectedType = t.name;
            return true;
        }
    }
    return false;
}

std::string ImageAnalyse::Processor::photoObjectType() const
{
    return d->m_detectedType;
}

void ImageAnalyse::Processor::addType(const std::string& type)
{
    ImageAnalyse::ImageType typ;
    typ.name = type;

    // Check if type already exist
    auto exist = std::binary_search(d->m_types.begin(), d->m_types.end(), typ, [](auto& t_a, auto& t_b){ return t_a.name < t_b.name;});
    if (exist) return;

    // Add if not exist
    d->m_types.push_back(typ);

    // Sort to detect faster
    std::sort(d->m_types.begin(), d->m_types.end(), [](auto& t_a, auto& t_b){ return t_a.name < t_b.name;});
}

bool ImageAnalyse::Processor::removeType(const std::string& type)
{
    // Check if type not exist
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.name == type); });
    if (pos == d->m_types.end())
        return false;

    d->m_types.erase(pos);
    return true;
}

std::vector<std::string> ImageAnalyse::Processor::availableTypes() const
{
    // Copy type names from internal vector
    std::vector<std::string> output;
    output.resize(d->m_types.size());
    for (size_t pos = 0; pos < d->m_types.size(); pos++)
        output[pos] = d->m_types[pos].name;
    return output;
}

void ImageAnalyse::Processor::setupType(const std::string& type, const std::string& templateFile)
{
    // Find type and setup it, if found
    auto pos = std::find_if(d->m_types.begin(), d->m_types.end(), [&](auto& t){ return (t.name == type); });
    if (pos != d->m_types.end())
        pos->setTemplate(templateFile);
}
