#ifndef IMAGETEMPLATE_H
#define IMAGETEMPLATE_H

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

namespace Analyse
{

class ImageTemplate
{
public:
    ImageTemplate();
    ImageTemplate(const ImageTemplate& imgTemplate);
    ~ImageTemplate();

    ImageTemplate& operator =(const ImageTemplate& imgTemplate);

    void setTemplate(const std::string& filepath);
    std::string getTemplate() const;

    void setName(const std::string& name);
    std::string getName() const;

    double match(const std::string& filepath);
    double matchLoaded(cv::Mat& img);

private:
    std::string m_templateFilePath; // Path to localfile
    cv::Mat m_loadedTemplateImage; // Main image
    std::vector<cv::Mat> m_templateRotations; // Optimisation for next time compare
    std::string m_templateName {"Unknown"}; // Name for template, registered in system

    void setupRotations();
    double match(cv::Mat& img, cv::Mat& templateImage);
    void createRotations(size_t& currentIndex);
};

}
#endif // IMAGETEMPLATE_H
