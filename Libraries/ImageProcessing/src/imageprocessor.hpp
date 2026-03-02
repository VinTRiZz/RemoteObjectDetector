#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <functional>
#include <memory>

#include <ROD/DetectionObject.h>

namespace ImageProcessing
{

/**
 * @brief The Processor class Класс обработки входящих изображений
 * @note Имеет внутренний пул потоков
 */
class Processor
{
public:
    Processor(unsigned int processorThreadCount);
    ~Processor();

    /**
     * @brief setImageCallback  Задать обработчик для оповещения об окончании анализа
     * @param cbk               Колбек. Параметры: идентификатор анализа, обнаруженный объект
     */
    void setImageCallback(std::function<void(const std::string&, const DataObjects::DetectionObject&)>&& cbk);

    /**
     * @brief addImage  Начать обработку изображения на основе его данных
     * @param analyseId ID анализа для последующей обработки результата
     * @param imageData Данные изображения (сырые байты)
     */
    void addImage(const std::string& analyseId, std::vector<uint8_t>&& imageData);

    void start();
    bool isWorking() const;
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};


}
