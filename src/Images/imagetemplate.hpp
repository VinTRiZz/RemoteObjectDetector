#ifndef IMAGETEMPLATE_H
#define IMAGETEMPLATE_H

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

namespace Analyse
{

enum ImageCompareMethod
{
    IMAGE_COMPARE_METHOD_TEMPLATE,
    IMAGE_COMPARE_METHOD_HIST,
    IMAGE_COMPARE_METHOD_CONTOUR
};

typedef std::vector<std::vector<cv::Point>> ContoursType;

class ImageComparator
{
public:
    ImageComparator();
    ImageComparator(const ImageComparator& imgTemplate);
    ~ImageComparator();

    ImageComparator& operator =(const ImageComparator& imgTemplate);

    void setTemplate(const std::string& filepath);
    std::string getTemplate() const;

    void setName(const std::string& name);
    std::string getName() const;

    double bestMatch(cv::Mat& img, ImageCompareMethod compMethod = ImageCompareMethod::IMAGE_COMPARE_METHOD_HIST);

private:
    std::string m_typeName {"Unknown"}; // Name for template, registered in system
    std::string m_templateFilePath; // Path to localfile template image

    cv::Mat m_loadedTemplateImage; // Main template image
    std::vector<cv::Mat> m_templateRotations; // Optimisation for next time compare
    ContoursType m_contours; // Contours based on rotations

    double matchTemplate(cv::Mat& img);
    double matchHist(cv::Mat& img);
    double matchContour(cv::Mat& img);

    void setupRotations();
    double match(cv::Mat& img, cv::Mat& templateImage);
    void createRotations(size_t& currentIndex);
};

}
#endif // IMAGETEMPLATE_H
