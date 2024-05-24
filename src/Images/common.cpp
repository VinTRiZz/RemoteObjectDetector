#include "common.hpp"
#include "logging.hpp"

// Functions not used directly in common,
// but used in function implementations
#include "commonsubfunctions.cpp"

namespace Common
{

cv::Mat loadImage(const std::string &filepath)
{
    // Read image
    cv::Mat imageRead = cv::imread(filepath, cv::IMREAD_GRAYSCALE);
    if (imageRead.empty())
    {
        LOG_OPRES_ERROR("Can't read template by path: %s", filepath.c_str());
        return {};
    }
//    cv::imwrite("image.png", imageRead);

    // Able morpho filter
    cv::Mat resultImage;
//    cv::adaptiveThreshold(imageRead, resultImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);
//    cv::imwrite("result_aThreshold.png", resultImage);

//    cv::Mat tempImage;
//    cv::morphologyEx(imageRead, resultImage, cv::MORPH_GRADIENT, tempImage);
//    cv::imwrite("result_morpho.png", resultImage);

//    cv::Laplacian(imageRead, resultImage, 5);
//    cv::imwrite("result_lapla.png", resultImage);

//    cv::Canny(imageRead, resultImage, 200, 100, 3);
//    cv::imwrite("result_canny.png", resultImage);

//    cv::Mat markersImage = cv::imread("shapedM.png");
//    cv::watershed(imageRead, markersImage);
//    cv::imwrite("result_markers.png", imageRead);
    return imageRead;
}

std::vector<cv::Mat> getObjects(const cv::Mat &image)
{
    std::vector<cv::Mat> result;
    try
    {
        // Image binarization
//        cv::threshold(mainImage, mainImage, 155, 255, cv::THRESH_BINARY);
//        cv::adaptiveThreshold(mainImage, mainImage, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 3, 0);
        cv::adaptiveThreshold(image, image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);

        // Search for objects
        std::vector<Common::ContourType> contours;
        Common::addContours(image, contours);

        result.resize(contours.size());
        size_t currentIndex = 0;

        // Describes how small object can be to process it
        constexpr size_t MINIMAL_OBJECT_SIZE = 30 * 30;

        // Get objects array from contours
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);
            if (boundingRect.area() < MINIMAL_OBJECT_SIZE) continue;
            result[currentIndex] = image(boundingRect);
        }

        // Remove unused positions
        auto resultEndR = std::find_if(result.rbegin(), result.rend(), [](auto& img){ return !img.empty(); });
        auto resultEnd = resultEndR.base();
        if (resultEnd != result.end()) result.erase(resultEnd + 1, result.end()); // +1 because iterator points to non-zero object

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
}

CompareMethod detectBestCompareMethod(const cv::Mat &image)
{
    return CompareMethod::COMPARE_METHOD_TEMPLATE;
}

void loadObjects(const std::string &path, std::list<TypeInfoHolder> &typeList)
{
    // Check if directory exist and it's directory
    if (!stdfs::exists(path) || !stdfs::is_directory(path))
    {
        LOG_ERROR("Invalid directory: %s", path.c_str());
        return;
    }

    // Iterate in directory
    for (const auto& dirent : stdfs::directory_iterator(path))
    {
        // If a file, try to get image from it
        if (stdfs::is_regular_file(dirent.path()))
        {
            std::string newTypeName = dirent.path().filename();
            newTypeName.erase(newTypeName.find_last_of('.'), newTypeName.size() -1);

            // Setup type as a name of file
            addType(typeList, newTypeName);
            auto typeIt = typeList.begin();
            typeIt->imagePath = dirent.path();
            setupInfoHolder(*typeIt);
        }
    }
}

}
