#include "utility.hpp"

namespace ImageProcessing::Utility
{

std::vector<uint8_t> serializeMat(const cv::Mat &mat) {
    int rows = mat.rows;
    int cols = mat.cols;
    int type = mat.type();
    size_t dataSize = mat.total() * mat.elemSize();

    // 2. space for header + data
    std::vector<uchar> buffer(sizeof(int) * 3 + dataSize);
    uchar* ptr = buffer.data();

    // 3. header
    memcpy(ptr, &rows, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &cols, sizeof(int));
    ptr += sizeof(int);
    memcpy(ptr, &type, sizeof(int));
    ptr += sizeof(int);

    // 4. pixel data
    memcpy(ptr, mat.data, dataSize);

    return buffer;
}

cv::Mat deserializeMat(const std::vector<uint8_t> &buffer) {
    // 1. Read header
    const uchar* ptr = buffer.data();
    int rows, cols, type;
    memcpy(&rows, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&cols, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&type, ptr, sizeof(int));
    ptr += sizeof(int);

    // 2. Create Mat with header info
    cv::Mat mat(rows, cols, type);
    size_t dataSize = mat.total() * mat.elemSize();

    // 3. Copy pixel data
    memcpy(mat.data, ptr, dataSize);
    return mat;
}

cv::Mat generateColorBarImage(int width, int height) {
    cv::Mat img(height, width, CV_8UC3);
    int barWidth = width / 8;
    std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 255, 255), // white
        cv::Scalar(255, 255, 0),   // yellow
        cv::Scalar(0, 255, 255),   // cyan
        cv::Scalar(0, 255, 0),     // green
        cv::Scalar(255, 0, 255),   // magenta
        cv::Scalar(255, 0, 0),     // red
        cv::Scalar(0, 0, 255),     // blue
        cv::Scalar(0, 0, 0)        // black
    };
    for (int i = 0; i < 8; ++i) {
        cv::Rect roi(i * barWidth, 0, barWidth, height);
        img(roi) = colors[i];
    }
    return img;
}



}
