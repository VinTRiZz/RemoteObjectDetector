#pragma once

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>

namespace Adaptors
{

/**
 * @brief The AdaptorStatus enum    Статус адаптера камеры
 */
enum class AdaptorStatus : short
{
    READY = 0,
    ERROR,
    BUSY
};

/**
 * @brief The CameraAdaptor class   Адаптер камеры, позволяющий получать изображения с камеры
 */
class CameraAdaptor
{
public:
    CameraAdaptor(const std::string& deviceFile = "/dev/video0");
    ~CameraAdaptor();

    /**
     * @brief status    Получить статус драйвера
     * @return          Текущий статус
     */
    AdaptorStatus status();

    /**
     * @brief shot          Запросить снимок с камеры в файл
     * @param outputFile    Абсолютный путь до файла
     * @return              true если камера доступна и снимок выполнен
     */
    bool shot(const std::string& outputFile);

    /**
     * @brief shotToBuffer  Запросить снимок с камеры в буффер
     * @param imageBuffer   Буффер для сохранения изображения
     * @return              true если камера доступна и снимок выполнен
     */
    bool shotToBuffer(cv::Mat& imageBuffer);

    /**
     * @brief setCameraDevice   Задать устройство для использования как камеры
     * @param cameraDevicePath  Абсолютный путь в Linux, например /dev/video0
     */
    void setCameraDevice(const std::string& cameraDevicePath);

    /**
     * @brief getCameraDevice   Получить текущее устройство-камеру
     * @return                  Путь до устройства. Например, /dev/video0
     */
    std::string getCameraDevice() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
    
}
