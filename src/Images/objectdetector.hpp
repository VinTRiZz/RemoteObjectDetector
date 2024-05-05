#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <memory>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

namespace Analyse
{

class ObjectDetector
{
public:
    ObjectDetector();
    ~ObjectDetector();

    void init();
    bool canWork() const;

    void setWeightsFile(const std::string& filepath);

    std::vector<cv::Mat> getObjects(const std::string &imageFullPath);

private:
    bool m_canWork {false};
    std::string m_weightsPath;
};

}

#endif // OBEJCTDETECTOR_H
