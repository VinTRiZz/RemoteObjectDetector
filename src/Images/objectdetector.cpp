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
        cv::Mat img = cv::imread("test/normal2.png");
        if (img.empty())
        {
            LOG_ERROR("Image load error");
            return {};
        }

        cv::Mat greyCopy;
        cv::cvtColor(img, greyCopy, cv::COLOR_BGR2GRAY);

        cv::threshold(greyCopy, greyCopy, 128, 255, cv::THRESH_BINARY);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(greyCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::Mat res = img.clone();
        cv::drawContours(res, contours, -1, cv::Scalar(0, 255, 0), 2);
//        cv::imwrite("test/normal2-found.png", res);

        int no = 1;
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);

            cv::Mat contouredObject = cv::Mat::zeros(boundingRect.height, boundingRect.width, img.type());
            // Don't write data yet
//            cv::imwrite(std::string("test/normal2-found-") + std::to_string(no) + ".png", contouredObject);
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
