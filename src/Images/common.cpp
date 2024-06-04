#include "common.hpp"

// Functions not used directly in common,
// but used in function implementations
#include "commonsubfunctions.cpp"

namespace Common
{

cv::Mat loadImage(const std::string &filepath)
{
    // Read image
    cv::Mat imageRead = cv::imread(filepath);
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


std::vector<double> createMoments(const cv::Mat& image)
{
    cv::Mat grayImage;
    if ((image.channels() > 2) && (image.type() != CV_8UC1))
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    else
        grayImage = image;

    // Get Hu moments
    std::vector<double> momentsVect;
    cv::HuMoments(cv::moments(grayImage), momentsVect);
    return momentsVect;
}


std::vector<cv::Mat> getObjects(const cv::Mat &targetImage, cv::Ptr<cv::BackgroundSubtractor>& pBacgroundSub)
{
    try
    {
        // Apply background erase
        cv::Mat objectsOnImage;
        pBacgroundSub->apply(targetImage, objectsOnImage, 0);

        // Search for objects
        std::vector<Common::ContourType> contours;
        Common::addContours(objectsOnImage, contours);

        std::vector<cv::Mat> result (contours.size());
        size_t currentIndex = 0;

        auto targetCopy = targetImage;

        // Get objects array from contours
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);
            if (boundingRect.area() < (30 * 30)) continue;

            cv::rectangle(targetCopy, boundingRect, cv::Scalar(0, 0, 255), 4);

            auto& image = result[0];
            cv::Mat mask = cv::Mat::zeros(image.size(), image.type());
            cv::drawContours(mask, contour, -1, 255, 0);
            cv::bitwise_not(mask, mask);

            cv::Mat result;
            image.copyTo(result, mask);
            cv::imwrite("result.jpg", result);

            auto img = targetImage(boundingRect); // Crop image
            result[currentIndex] = img;
        }

        return result;

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }
    return {};
}

void loadObjects(const std::string &path, std::list<TypeInfoHolder> &typeList, cv::Ptr<cv::BackgroundSubtractor>& pBackgroundSub)
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
            setupInfoHolder(*typeIt, pBackgroundSub);
        }
    }
}

}
