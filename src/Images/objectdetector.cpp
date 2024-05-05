#include "objectdetector.hpp"

#include "logging.hpp"

#include "common.hpp"

namespace Analyse
{

constexpr size_t MINIMAL_OBJECT_SIZE = 30 * 30;

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

std::vector<cv::Mat> ObjectDetector::getObjects(const std::string& imageFullPath)
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

        Common::ContoursType contours2;
        for (auto& contour : contours)
        {
            cv::Rect boundingRect = cv::boundingRect(contour);

            if (boundingRect.area() < MINIMAL_OBJECT_SIZE)
                continue;

            contours2.push_back(contour);
            result.push_back(mainImage(boundingRect));
        }

        Common::drawFound(mainImage, contours2, basepath);

    } catch (std::exception& ex) {
        LOG_ERROR("Got OpenCV exception: %s", ex.what());
    }

    return result;
}

}
