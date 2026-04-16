#include "videoreader.hpp"

#include <opencv2/opencv.hpp>

#include <cstring>

#include <Components/Logger/Logger.h>

#include <ROD/ImageProcessing/Utility.h>

namespace ImageProcessing {

struct VideoReader::Impl
{
    std::string videoFilePath;
    cv::VideoCapture cap;
    int     width {};
    int     height {};
    double  fps {};
    int     frameCount {};
};

VideoReader::VideoReader() :
    d {new Impl()}
{
    d->cap.setExceptionMode(true);
}

VideoReader::~VideoReader()
{

}

bool VideoReader::setVideofile(const std::string &videoFilePath)
{
    try {
        d->cap.open(videoFilePath);
    } catch (const cv::Exception& ex) {
        COMPLOG_ERROR("Failed to open video file:", videoFilePath, "REASON:", std::string(ex.what())); // rly strange 'what()' method calling (but works)
        return false;
    }
    d->videoFilePath = videoFilePath;

    // Video properties
    d->width  = static_cast<int>(d->cap.get(cv::CAP_PROP_FRAME_WIDTH));
    d->height = static_cast<int>(d->cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    d->fps    = d->cap.get(cv::CAP_PROP_FPS);
    d->frameCount = static_cast<int>(d->cap.get(cv::CAP_PROP_FRAME_COUNT));

    // corner case
    if (d->frameCount <= 0) {
        // Вычисляем приблизительно по длительности
        double totalMs = d->cap.get(cv::CAP_PROP_POS_MSEC);
        if (totalMs > 0 && d->fps > 0) {
            d->frameCount = static_cast<int>((totalMs / 1000.0) * d->fps);
        }
    }
    return true;
}

std::string_view VideoReader::getVideofile() const
{
    return d->videoFilePath;
}

int VideoReader::getWidth() const
{
    return d->width;
}

int VideoReader::getHeight() const
{
    return d->height;
}

double VideoReader::getFps() const
{
    return d->fps;
}

int VideoReader::getFrameCount() const
{
    return d->frameCount;
}

VideoReader::Iterator VideoReader::begin()
{
    return Iterator(&d->cap, d->frameCount);
}

VideoReader::Iterator VideoReader::end()
{
    return Iterator();
}


// ================================ ITERATOR ============================== //
VideoReader::Iterator::Iterator() :
    m_cap(nullptr),
    m_currentFrameIndex(0),
    m_totalFrames(0),
    m_endOfVideo(true)
{

}

VideoReader::Iterator::Iterator(cv::VideoCapture *cap, int totalFrames) :
    m_cap(cap),
    m_currentFrameIndex(0),
    m_totalFrames(totalFrames),
    m_endOfVideo(false)
{
    readNextFrame();
}

void VideoReader::Iterator::readNextFrame()
{
    if (!m_cap || !m_cap->isOpened()) {
        m_endOfVideo = true;
        return;
    }

    cv::Mat frame;
    if (!m_cap->read(frame)) {
        m_endOfVideo = true;
        return;
    }

    m_currentFrameData = Utility::serializeMat(frame);
    ++m_currentFrameIndex;
}

VideoReader::Iterator::reference VideoReader::Iterator::operator*() const {
    return m_currentFrameData;
}

VideoReader::Iterator::pointer VideoReader::Iterator::operator->() const {
    return &m_currentFrameData;
}

VideoReader::Iterator& VideoReader::Iterator::operator++() {
    if (!m_endOfVideo) {
        readNextFrame();
    }
    return *this;
}

void VideoReader::Iterator::operator++(int) {
    ++(*this);
}

bool VideoReader::Iterator::operator==(const Iterator& other) const {
    if (m_endOfVideo && other.m_endOfVideo) {
        return true;
    }
    return m_cap == other.m_cap &&
           m_currentFrameIndex == other.m_currentFrameIndex;
}

bool VideoReader::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

} // namespace ImageProcessing
