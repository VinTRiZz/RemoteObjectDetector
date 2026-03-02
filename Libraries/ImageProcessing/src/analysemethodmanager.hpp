#pragma once

#include "typesholder.hpp"

namespace Analyse
{

class AnalyseMethodManager
{
public:
    AnalyseMethodManager(TypesHolder& typesHolder);

    std::list<FoundObjects > detectObject(const cv::Mat& image);

private:
    TypesHolder& m_typesHolder;

    double compareTest(const TypeInfoHolder& typeIHolder, const cv::Mat& image);

    double compareMoments(const TypeInfoHolder &typeIHolder, const cv::Mat& image);
    double compareHistogram(const TypeInfoHolder& typeIHolder, const cv::Mat& image);
    double compareTemplate(const TypeInfoHolder& typeIHolder, const cv::Mat& image);
};

}
