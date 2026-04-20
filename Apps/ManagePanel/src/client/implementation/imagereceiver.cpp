#include "imagereceiver.hpp"

#include <Components/Network/ServerUDP.h>
#include <Components/Logger/Logger.h>

#include <ROD/Protocol.h>
#include <ROD/ImageProcessing/Utility.h>

#include <QImage>
#include <opencv2/opencv.hpp>

namespace {

/**
 * Convert an OpenCV cv::Mat to a Qt QImage
 *
 * Supported input types:
 * - CV_8UC1 (grayscale)       → QImage::Format_Grayscale8
 * - CV_8UC3 (BGR)             → QImage::Format_RGB888 (after swapping R and B)
 * - CV_8UC4 (BGRA)            → QImage::Format_RGBA8888 (after swapping R and B)
 *
 * If the input matrix is empty, returns a null QImage.
 * The function copies the pixel data, so the QImage is independent of the original cv::Mat.
 *
 * @param mat The input OpenCV matrix
 * @return QImage containing the same image data
 */
QImage cvMatToQImage(const cv::Mat &mat) {
    if (mat.empty())
        return QImage();

    switch (mat.type()) {
    case CV_8UC1: {
        // Grayscale image
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return image.copy();
    }

    case CV_8UC3: {
        // BGR → convert to RGB by swapping channels
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        QImage image(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        return image.copy();
    }

    case CV_8UC4: {
        // BGRA → convert to RGBA by swapping R and B
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        QImage image(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888);
        return image.copy();
    }

    default:
        // Unsupported format – return null image
        return QImage();
    }
}

}


struct ImageReceiver::Impl
{
    UDP::Server streamingServer;
};

ImageReceiver::ImageReceiver(QObject *parent) :
    QObject{parent},
    d {new Impl()}
{
    d->streamingServer.setRequestProcessor([this](std::vector<uint8_t>&& receivedData){
        // TODO: Process packet data

        std::string str;
        std::copy(receivedData.begin(), receivedData.end(), std::back_inserter(str));
        COMPLOG_DEBUG("GOT UDP PACKET:", str);

        // TODO: Remove (debug solution)
        auto testImg = ImageProcessing::Utility::generateColorBarImage(120, 250);
        QImage img = cvMatToQImage(testImg);
        emit imageReceived(img);
    });
}

ImageReceiver::~ImageReceiver()
{

}

void ImageReceiver::startListen(uint16_t port)
{
    d->streamingServer.start(port);
}

bool ImageReceiver::isListening() const
{
    return d->streamingServer.isWorking();
}

void ImageReceiver::stopListen()
{
    d->streamingServer.stop();
}
