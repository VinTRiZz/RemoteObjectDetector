#include "objectdetector.hpp"

#include "logging.hpp"

#include "common.hpp"

namespace Analyse
{

ObjectDetector::ObjectDetector()
{

}

ObjectDetector::~ObjectDetector()
{

}

void ObjectDetector::init()
{
    // s
}

bool ObjectDetector::canWork() const
{
    return m_canWork;
}

void ObjectDetector::setWeightsFile(const std::string &filepath)
{
    m_weightsPath = filepath;
}

std::vector<cv::Mat> ObjectDetector::getObjects(cv::Mat &mainImage)
{
    std::vector<cv::Mat> result;

    try
    {
        if (mainImage.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }

        cv::Mat mainThreshed;
        cv::threshold(mainImage, mainThreshed, 128, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mainThreshed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours(mainThreshed, contours, -1, cv::Scalar(0, 255, 0), 2);

        int no = 1;
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);
            cv::Mat contouredObject = cv::Mat::zeros(boundingRect.height, boundingRect.width, mainImage.type());
            cv::imwrite(std::string("temp/Object-") + std::to_string(no) + ".png", contouredObject);
        }

    } catch (std::exception& ex)
    {
        LOG_ERROR(ex.what());
    }
    LOG_DEBUG("Success");
    exit(0);

    return result;
}

}
