#include "imageprocessor.hpp"

// Template matching from OpenCV
#include <opencv2/opencv.hpp>

// STD algorithms
#include <algorithm>

// Terminal data output
#include <iostream>

// Directory processing
#include <dirent.h>

// Log info
#include "logging.hpp"


namespace ImageAnalyse
{

struct ImageObject
{
private:
    // Path to template image
    std::string templatePath;

    // OpenCV object to handle images
    cv::Mat templateImage;

public:
    // Name (type) of image type
    std::string name;

    ImageObject() = default;
    ~ImageObject() = default;

    bool operator !=(const ImageObject& ot)
    {
        return (ot.name == name);
    }

    // Set path to template file
    void setTemplate(const std::string& filePath)
    {
        templateImage = cv::imread(filePath);
    }

    // Ask if object is actually the searching for, using match percent above that it's true
    bool isObject(const std::string& filePath, const double matchPercent = 0.8)
    {
        if (templateImage.empty())
            return false;

        // Get image
        cv::Mat img = cv::imread(filePath);
        cv::Mat result;

        if (img.empty())
            return false;

        // Compare to template
        cv::matchTemplate(img, templateImage, result, cv::TM_CCOEFF_NORMED); // TODO: Experiment with comp methods

        // Setup min-max loc
        double minVal {}, maxVal {};
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal >= matchPercent)
        {
            LOG_OPRES_SUCCESS("Type [ %s ] match percent: [ %f ]", name.c_str(), maxVal * 100.0);
        }
        else
        {
            LOG_OPRES_ERROR("Type [ %s ] match percent: [ %f ]", name.c_str(), maxVal * 100.0);
        }

        // Check if image found
        if (maxVal < matchPercent)
            return false;
        return true;
    }
};

}

struct ImageAnalyse::Processor::AnalysatorPrivate
{
    std::string m_templatesDir;    // Path to directory with saved neural nets
    std::vector<ImageObject> m_types; // Contain types listed in config file in the same dir with neural nets
};

ImageAnalyse::Processor::Processor() :
    d {new AnalysatorPrivate()}
{

}

ImageAnalyse::Processor::~Processor()
{

}

void ImageAnalyse::Processor::setImageTemplateDir(const std::string& path)
{
    d->m_templatesDir = path;
    processTemplatesDirectory();  // Update contents
}

std::string ImageAnalyse::Processor::processPhoto(const std::string& imageFilePath, const double matchPercent)
{
    for (auto& t : d->m_types)
    {
        if (t.isObject(imageFilePath, matchPercent))
            return t.name;
    }
    return "Unknown";
}

void ImageAnalyse::Processor::addType(const std::string& type)
{
    ImageAnalyse::ImageObject typ;
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

void ImageAnalyse::Processor::processTemplatesDirectory()
{
    if (!stdfs::exists(d->m_templatesDir) || !stdfs::is_directory(d->m_templatesDir))
    {
        LOG_ERROR("Invalid directory: %s", d->m_templatesDir.c_str());
        return;
    }

    for (const auto& dirent : stdfs::directory_iterator(d->m_templatesDir))
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
