#ifndef ANALYSEMETHODMANAGER_HPP
#define ANALYSEMETHODMANAGER_HPP

#include "common.hpp"

namespace Analyse
{

class AnalyseMethodManager
{
public:
    AnalyseMethodManager();

    double compareMoments(const Common::TypeInfoHolder& typeIHolder, const cv::Mat& image);
    double compareHistogram(const Common::TypeInfoHolder& typeIHolder, const cv::Mat& image);
    double compareTemplate(const Common::TypeInfoHolder& typeIHolder, const cv::Mat& image);
};

}

#endif // ANALYSEMETHODMANAGER_HPP
