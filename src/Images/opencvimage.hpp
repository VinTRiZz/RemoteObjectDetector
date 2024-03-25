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

    // Return count of variations created
    void setupVariations(std::vector<cv::Mat>& templateImages);
    size_t setupRotations(int currentIndex);

    // Compare
    static double templateCompare(cv::Mat& img, cv::Mat& templateImg, bool withOutput = false);

    cv::Mat setupTemplate(const std::string& templateFilePath);

public:
    // Name (type) of image type
    std::string name;

    ImageObject() = default;
    ~ImageObject() = default;

    // Returns percent of template image match to image file
    float templateMatch(const std::string& imageFilePath, const std::string& templateFilePath);

    // Crop image to contour
    static cv::Mat cropToContour(cv::Mat &image);
};


}

#endif // OPENCV_IMAGE_HPP
