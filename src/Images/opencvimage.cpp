#include "opencvimage.hpp"

#include "logging.hpp"

// Faster comparing
#include <future>

bool Analyse::ImageObject::operator !=(const Analyse::ImageObject &ot)
{
    return (ot.name == name);
}

void Analyse::ImageObject::setTemplate(const std::string &filePath)
{
    // Read image
    templateImages.push_back(cv::imread(filePath));
    if (templateImages[0].empty())
        templateImages.pop_back();
}

float Analyse::ImageObject::match(const std::string &filePath, float minimalMatch)
{
    if (!templateImages.size())
        return false;

    // Create image copyies to compare
    int currentIndex = 1;

    // Mirror vertically
    templateImages.push_back({});
    cv::flip(templateImages[0], templateImages[currentIndex], 0);
    currentIndex++;

    // Mirror horizontally
    templateImages.push_back({});
    cv::flip(templateImages[0], templateImages[currentIndex], 1);
    currentIndex++;

    // Rotate 90
    templateImages.push_back({});
    cv::transpose(templateImages[0], templateImages[currentIndex]);
    cv::flip(templateImages[0], templateImages[currentIndex], 1);
    currentIndex++;

    // Rotate -90
    templateImages.push_back({});
    cv::transpose(templateImages[0], templateImages[currentIndex]);
    cv::flip(templateImages[0], templateImages[currentIndex], 1);
    currentIndex++;

    // Rotate 180
    templateImages.push_back({});
    cv::flip(templateImages[0], templateImages[currentIndex], -1);
    currentIndex++;

    cv::Mat compareImage = cv::imread(filePath);
    std::pair<int, double> indexMatch {0, 0};
    std::vector<std::future<double>> futures;

    // Compare using image copyies
    for (int i = 0 ; i < currentIndex; i++)
    {
        futures.push_back(std::async([&compareImage, i, minimalMatch, &indexMatch, this](){
            if (compareImage.empty())
            {
                LOG_OPRES_ERROR("Can't read image");
                return double();
            }

            cv::Mat result;
            double minVal {}, maxVal {};
            cv::Point minLoc, maxLoc;

            // Compare
            if (maxVal > minimalMatch) return maxVal;
            //                cv::matchTemplate(compareImage, templateImages[i], result, cv::TM_CCOEFF_NORMED);
            cv::matchTemplate(compareImage, templateImages[i], result, cv::TM_CCORR_NORMED);
            //                cv::matchTemplate(compareImage, templateImages[i], result, cv::TM_SQDIFF_NORMED);
            if (maxVal > minimalMatch) return maxVal;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            if (maxVal > minimalMatch) return maxVal;

            if (maxVal > indexMatch.second) indexMatch = std::make_pair(i, maxVal);

            // Check compare result
            return maxVal;
        }));
    }

    for (auto& fut : futures)
    {
        if (fut.valid())
        {
            auto tempRes = fut.get();
            if (tempRes > indexMatch.second) indexMatch.second = tempRes;
        }
    }

    LOG_INFO("[ %s ] Match percent: %f", this->name.c_str(), indexMatch.second);

    return indexMatch.second;
}
