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
    //        return result;
}

}
