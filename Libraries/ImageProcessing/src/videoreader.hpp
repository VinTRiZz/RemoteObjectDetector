#pragma once

#include <memory>
#include <iterator>

#include <ROD/ImageProcessing/Common.h>

namespace cv {
class Mat;
class VideoCapture;
}

namespace ImageProcessing {

/**
 * @brief The VideoReader class Video file reader for testing
 */
class VideoReader
{
public:
    VideoReader();
    ~VideoReader();

    bool setVideofile(const std::string& videoFilePath);
    std::string_view getVideofile() const;

    // Video properties
    int     getWidth() const;
    int     getHeight() const;
    double  getFps() const;
    int     getFrameCount() const;

    // Iterator support
    class Iterator;
    Iterator begin();
    Iterator end();

private:
    struct Impl;
    std::shared_ptr<Impl> d;

    friend class Iterator;
};


/**
 * @brief The VideoReader::Iterator class Frame iterator
 */
class VideoReader::Iterator {
public:
    using value_type = ImageData_t;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::input_iterator_tag;

    // Constructors
    Iterator();
    Iterator(cv::VideoCapture* cap, int totalFrames);

    // Move-only iterator
    Iterator(const Iterator&) = delete;
    Iterator& operator=(const Iterator&) = delete;
    Iterator(Iterator&&) = default;
    Iterator& operator=(Iterator&&) = default;

    // Iterator operators
    reference   operator*() const;
    pointer     operator->() const;
    Iterator&   operator++();
    void        operator++(int);
    bool        operator==(const Iterator& other) const;
    bool        operator!=(const Iterator& other) const;

private:
    cv::VideoCapture* m_cap {nullptr};
    int             m_currentFrameIndex {};
    int             m_totalFrames {};
    value_type      m_currentFrameData;
    bool            m_endOfVideo {true};

    void readNextFrame();
};

} // namespace ImageProcessing
