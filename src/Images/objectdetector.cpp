#include "objectdetector.hpp"

#include "logging.hpp"

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
        cv::Mat img = cv::imread("test/normal.png");
        if (img.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }
        LOG_DEBUG("NH 0");

        cv::Mat greyCopy;
        cv::cvtColor(img, greyCopy, cv::COLOR_BGR2GRAY);
        LOG_DEBUG("NH 1");

        cv::threshold(greyCopy, greyCopy, 128, 255, cv::THRESH_BINARY);
        LOG_DEBUG("NH 2");

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(greyCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        LOG_DEBUG("NH 3");

        cv::Mat res = img.clone();
        cv::drawContours(res, contours, -1, cv::Scalar(0, 255, 0), 2);
        cv::imwrite("test/normal-found.png", res);
    } catch (std::exception& ex)
    {
        LOG_ERROR(ex.what());
    }
    LOG_DEBUG("Success");
    exit(0);

    return result;
}

}
