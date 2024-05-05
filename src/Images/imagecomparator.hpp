#ifndef IMAGETEMPLATE_H
#define IMAGETEMPLATE_H

// OpenCV header
#include <opencv2/opencv.hpp>

// Data containers
#include <string>

namespace Analyse
{

// Compare method for OpenCV functions use
enum ImageCompareMethod
{
    IMAGE_COMPARE_METHOD_TEMPLATE,  // Compares using image as template image
    IMAGE_COMPARE_METHOD_HIST,      // Compares creating histogram from images
    IMAGE_COMPARE_METHOD_CONTOUR    // Compares using contours created from images
};

// Equal to Common::ContoursType
typedef std::vector<std::vector<cv::Point>> ContoursType;


class ImageComparator
{
public:
    // Constructors
    ImageComparator();
    ImageComparator(const ImageComparator& imgTemplate);
    ~ImageComparator();

    // Operators
    ImageComparator& operator =(const ImageComparator& imgTemplate);

    // Set image by path, used to create contours and rotations of it
    // to compare then with other images using function bestMatch
    void setImage(const std::string& filepath);
    std::string getImagePath() const;

    void setThemeImage(const cv::Mat& themeImg);

    // Set name of type this comparator will work with,
    // used to identify objects
    void setName(const std::string& name);
    std::string getName() const;

    // Compares images using method provided
    double bestMatch(cv::Mat& img, ImageCompareMethod compMethod = ImageCompareMethod::IMAGE_COMPARE_METHOD_HIST);

private:
    std::string m_typeName {"Unknown"}; // Name for template, registered in system
    std::string m_templateFilePath; // Path to localfile template image

    cv::Mat m_loadedTemplateImage; // Main template image

    // Containers setted up after first calling of compare,
    // later they contains result of generation to compare faster
    std::vector<cv::Mat> m_templateRotations;   // Rotations of template image
    std::vector<cv::Mat> m_histograms;          // Histograms calculated for template image
    ContoursType m_contours;                    // Contours based on rotations

    double matchTemplate(cv::Mat& img);
    double matchHist(cv::Mat& img);
    double matchContour(cv::Mat& img);

    void setupRotations();
    void createRotations(size_t& currentIndex);
    void setupHistograms();
};

}
#endif // IMAGETEMPLATE_H
