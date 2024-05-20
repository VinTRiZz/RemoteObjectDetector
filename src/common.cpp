#include "common.hpp"

#include "logging.hpp"

namespace Common
{

// Describes how small object can be to process it
constexpr size_t MINIMAL_OBJECT_SIZE = 30 * 30;

cv::Mat loadImage(const std::string &filepath)
{
    // Read image
    cv::Mat result = cv::imread(filepath);
    if (result.empty())
    {
        LOG_OPRES_ERROR("Can't read template by path: %s", filepath.c_str());
        return {};
    }

    // Convert image to gray to better search
    cv::Mat greyResult;
    cv::cvtColor(result, greyResult, cv::COLOR_BGR2GRAY);
    return greyResult;
}

void addContours(cv::Mat &img, ContoursType &imageContours)
{
    // Search into temporary array
    ContoursType tempRes;
    cv::findContours(img, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // If nothing found, return
    if (!tempRes.size())
        return;

    // Search for largest contour
    std::vector<cv::Point>* largestContour;
    double currentArea {0};

    // If nothing found or just one countour in array, will be set to first
    largestContour = &tempRes[0];
    for (auto& contour : tempRes)
    {
        // Count area of contour
        double tempArea = cv::contourArea(contour);
        if (currentArea < tempArea)
        {
            currentArea = tempArea;
            largestContour = &contour;
        }
    }

    // Add only largest contour into array
    imageContours.push_back(*largestContour);
}

void drawFound(cv::Mat &img, ContoursType &foundObjectContours, const std::string &baseName)
{
    // Save image objects
    int no = 1;
    for (auto& contour : foundObjectContours)
    {
        cv::Rect boundingRect = cv::boundingRect(contour);
        cv::Mat contouredObject = img(boundingRect);
        cv::imwrite(baseName + "_Object-" + std::to_string(no++) + ".png", contouredObject);
    }
}

std::vector<cv::Mat> getObjects(const std::string &imageFullPath)
{
    std::vector<cv::Mat> result;
    try
    {
        // Get image
        cv::Mat mainImage = Common::loadImage(imageFullPath);
        if (mainImage.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }

        // Image binarization (set bytes with lower 128 values to 0, upper to 1)
        cv::Mat mainThreshed;
        cv::threshold(mainImage, mainThreshed, 127, 255, cv::THRESH_BINARY);

        // Search for objects
        Common::ContoursType contours;
        Common::addContours(mainThreshed, contours);

        // Get objects array from contours
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);

            if (boundingRect.area() < MINIMAL_OBJECT_SIZE)
                continue;
            result.push_back(mainImage(boundingRect));
        }

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
}

}
