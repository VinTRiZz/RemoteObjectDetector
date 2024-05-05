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

    cv::Mat greyResult;
    cv::cvtColor(result, greyResult, cv::COLOR_BGR2GRAY);
    return greyResult;
}

void addContours(cv::Mat &img, ContoursType &imageContours)
{
    ContoursType tempRes;
    cv::findContours(img, tempRes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (!tempRes.size())
    {
        LOG_DEBUG("Not found contours");
        return;
    }

    std::vector<cv::Point>* largestContour;
    double currentArea {0};

    largestContour = &tempRes[0];
    for (auto& contour : tempRes)
    {
        double tempArea = cv::contourArea(contour);
        if (currentArea < tempArea)
        {
            currentArea = tempArea;
            largestContour = &contour;
        }
    }

    imageContours.push_back(*largestContour);
}

void drawFound(cv::Mat &img, ContoursType &foundObjectContours, const std::string &baseName)
{
    cv::Mat resultImg = img.clone();
    int no = 1;
    for (auto& contour : foundObjectContours)
    {
        cv::Rect boundingRect = cv::boundingRect(contour);
        cv::Mat contouredObject = resultImg(boundingRect);
        cv::imwrite(baseName + "_Object-" + std::to_string(no++) + ".png", contouredObject);
    }
}

std::vector<cv::Mat> getObjects(const std::string &imageFullPath)
{
    std::vector<cv::Mat> result;
    try
    {
        cv::Mat mainImage = Common::loadImage(imageFullPath);

        if (mainImage.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }

        cv::Mat mainThreshed;
        cv::threshold(mainImage, mainThreshed, 128, 255, cv::THRESH_BINARY);

        Common::ContoursType contours;
        Common::addContours(mainThreshed, contours);

        std::string basepath = "temp/";
        basepath += std::filesystem::path(imageFullPath).filename();
        basepath.erase(basepath.size() - 4, basepath.size());

        //        Common::ContoursType contours2;
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);

            if (boundingRect.area() < MINIMAL_OBJECT_SIZE)
                continue;

            //            contours2.push_back(contour);
            result.push_back(mainImage(boundingRect));
        }

        //        Common::drawFound(mainImage, contours2, basepath);

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
}

}
