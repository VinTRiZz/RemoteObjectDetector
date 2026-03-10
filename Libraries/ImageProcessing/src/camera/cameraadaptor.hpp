#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdint.h>

namespace ImageProcessing
{

/**
 * @brief The AdaptorStatus enum    Статус адаптера камеры
 */
enum class AdaptorStatus : short
{
    READY = 0,
    ERROR,
    BUSY,
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
     * @brief getStatus Получить статус драйвера
     * @return          Текущий статус
     */
    AdaptorStatus getStatus() const;

    /**
     * @brief shot          Запросить снимок с камеры в файл
     * @param outputFile    Абсолютный путь до файла
     * @return              true если камера доступна и снимок выполнен
     */
    bool shot(const std::string& outputFile);

    /**
     * @brief shot  Запросить снимок с камеры в буффер
     * @return      Пустой vector при ошибке
     */
    std::vector<uint8_t> shot();

    /**
     * @brief initStreaming     Подготовить стриминг данных с камеры, используя GStreamer
     * @param configuration     Конфигурация пайплайна для GStreamer
     */
    bool initStreaming(const std::string& configuration);
    bool initStreamingSimple(const std::string serverIp, uint16_t serverPort);
    bool streamShot();
    void deinitStreaming();

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
    bool canWork() const;
    void setStatus(AdaptorStatus ast);

    struct Impl;
    std::unique_ptr<Impl> d;
};
    
}
