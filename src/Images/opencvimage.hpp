#ifndef OPENCV_IMAGE_HPP
#define OPENCV_IMAGE_HPP

// Template matching from OpenCV
#include <opencv2/opencv.hpp>

#include <vector>

namespace Analyse
{


struct ImageObject
{
private:
    // Path to template image
    std::string templatePath;

    // OpenCV object to handle images
    std::vector<cv::Mat> templateImages;

public:
    // Name (type) of image type
    std::string name;

    ImageObject() = default;
    ~ImageObject() = default;

    bool operator !=(const ImageObject& ot);

    // Set path to template file
    void setTemplate(const std::string& filePath);

    // Ask if object is actually the searching for, using match percent above that it's true
    float match(const std::string& filePath, float minimalMatch);
};


}

#endif // OPENCV_IMAGE_HPP
