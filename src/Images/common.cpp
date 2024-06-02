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
    std::vector<cv::Mat> result;
    try
    {
        // Apply background erase
        cv::Mat objectsOnImage;
        pBacgroundSub->apply(targetImage, objectsOnImage);
        cv::imwrite("of_bb.png", objectsOnImage);

        cv::GaussianBlur(objectsOnImage, objectsOnImage, cv::Size(5, 5), 0);
        cv::imwrite("of_ab.png", objectsOnImage);
        cv::imwrite("of_input.png", targetImage);

        // Search for objects
        std::vector<Common::ContourType> contours;
        Common::addContours(objectsOnImage, contours);

        result.resize(contours.size());
        size_t currentIndex = 0;

        // Get objects array from contours
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);

            auto img = targetImage(boundingRect); // Crop image
            if (img.empty()) continue; // Image must not be empty

            result[currentIndex] = img;
        }

        // Remove empty images
        result.erase(std::remove_if(result.begin(), result.end(), [](auto& img){ return img.empty(); }));

        if (result.size() > 10)
        {
            LOG_DEBUG("Dohuya (%i)", result.size());
            return result;
        }

        LOG_DEBUG("Start");
        for (auto& res : result)
        {
            if (res.empty())
            {
                LOG_DEBUG("SHIT!");
            }
        }
        LOG_DEBUG("Complete");

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
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
