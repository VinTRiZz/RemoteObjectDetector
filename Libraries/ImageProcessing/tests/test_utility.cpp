#include <gtest/gtest.h>

#include "utility.hpp"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

bool equalBitwise(const cv::Mat& a, const cv::Mat& b) {
    if (a.size() != b.size() || a.type() != b.type())
        return false;

    int rows = a.rows;
    int cols = a.cols;
    size_t elemSize = a.elemSize();     // bytes per pixel (e.g., 3 for CV_8UC3)
    size_t rowBytes = cols * elemSize;  // bytes per row (excluding padding)

    for (int r = 0; r < rows; ++r) {
        const uchar* rowA = a.ptr<uchar>(r);
        const uchar* rowB = b.ptr<uchar>(r);
        if (std::memcmp(rowA, rowB, rowBytes) != 0)
            return false;
    }
    return true;
}

TEST(ImageProcessing_Utility, ImageSerialize) {
    auto img = ImageProcessing::Utility::generateColorBarImage(341, 992);
    ASSERT_FALSE(img.empty());

    auto serializedImg = ImageProcessing::Utility::serializeMat(img);
    auto deserializedImg = ImageProcessing::Utility::deserializeMat(serializedImg);

    ASSERT_TRUE(equalBitwise(img, deserializedImg));
}
