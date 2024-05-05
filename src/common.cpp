#include "common.hpp"

#include "logging.hpp"

namespace Common
{

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

}
