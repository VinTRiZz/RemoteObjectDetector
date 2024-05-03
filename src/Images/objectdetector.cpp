#include "objectdetector.hpp"

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



    return result;
}

}
