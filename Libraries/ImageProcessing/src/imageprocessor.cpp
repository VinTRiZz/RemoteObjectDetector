#include "imageprocessor.hpp"

#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>
#include <oneapi/tbb/parallel_for.h>

#include <Components/Logger/Logger.h>

namespace ImageProcessing
{

struct Processor::Impl
{
    tbb::task_arena taskArena;
    std::function<void(const std::string&, const DataObjects::DetectionObject&)> detectionCallback;
};

Processor::Processor(unsigned int processorThreadCount) :
    d {new Impl{processorThreadCount} }
{

}

Processor::~Processor()
{

}

void Processor::setImageCallback(std::function<void (const std::string &, const DataObjects::DetectionObject &)> &&cbk)
{
    d->detectionCallback = std::move(cbk);
}

void Processor::addImage(const std::string &analyseId, std::vector<uint8_t> &&imageData)
{
    d->taskArena.execute([] {
        tbb::task_group tg;

//        tg.run([] {
//            COMPLOG_INFO("Main task started");

//            // Имитация длительных вычислений
//            std::this_thread::sleep_for(std::chrono::seconds(2));

//            // В процессе задачи оповещаем, что можно выполнять другие дела
//            // Это достигается через запуск параллельных подзадач
//            tbb::parallel_for(tbb::blocked_range<int>(0, 5),
//                [](const tbb::blocked_range<int>& r) {
//                    COMPLOG_INFO("Other task started");
//                    for (int i = r.begin(); i < r.end(); ++i) {
//                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
//                    }
//                    COMPLOG_INFO("Other task complete");
//                }
//            );

//            COMPLOG_OK("Main task complete");
//        });

        tg.wait();
    });
}

void Processor::start()
{

}

bool Processor::isWorking() const
{
    return false;
}

void Processor::stop()
{

}

}
