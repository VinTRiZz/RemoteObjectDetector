#include "interface.h"

// Components
#include "imageprocessor.h"
#include "imageconverter.h"
#include "cameradriver.h"

// Status work
#include <atomic>

// Event work
#include <condition_variable>
#include <mutex>
#include <list>

#include <future>

// User output
#include <iostream>

#define PRINT_LOG(what) std::cout << "[LOG CHANNEL - INTERFACE: " << __FUNCTION__ << "] " << what << std::endl
#define PRINT_SUC(what) std::cout << "[LOG CHANNEL - INTERFACE: " << __FUNCTION__ << "] [\033[32m OK! \033[0m] " << what << std::endl
#define PRINT_ERR(what) std::cout << "[LOG CHANNEL - INTERFACE: " << __FUNCTION__ << "] [\033[31mERROR\033[0m] " << what << std::endl

namespace ObjectDetector
{

enum EventType
{
    ANY,
    STARTED,
    STOP,
    STOPPED
};

}

struct ObjectDetector::Interface::InterfacePrivate
{
    std::string m_ofpath; // Path to file with program output

    std::atomic<ObjectDetector::ProgramStatus> m_status {ObjectDetector::ProgramStatus::INIT_ERROR}; // Status of program

    std::condition_variable m_eventVar;
    std::mutex m_eventMutex;
    std::list<EventType> m_eventList;
    std::atomic<int> m_notifyCount {0}; // Used to avoid infinity signals
    std::future<void> m_workFut;

    void event(EventType e, bool maxPriority = false)
    {
        std::unique_lock<std::mutex> lock(m_eventMutex);

        if (maxPriority)
            m_eventList.push_front(e);
        else
            m_eventList.push_back(e);

        PRINT_LOG("Got event " << (int)e);
        m_eventVar.notify_one(); // Start notify chain
    }

    ObjectDetector::EventType waitForEvent(EventType e)
    {
        PRINT_LOG("Called for event " << (int)e);
        while (m_status.load() == ObjectDetector::ProgramStatus::IDLE)
        {
            std::unique_lock<std::mutex> lock(m_eventMutex);
            PRINT_LOG("Waiting for event " << (int)e);
            m_eventVar.wait(lock);
            PRINT_LOG("Read event " << (int)m_eventList.front());

            if ((e != ObjectDetector::EventType::ANY) && (e != m_eventList.front()) && (m_notifyCount.load() < m_eventList.size()))
            {
                // Remove deaf event
                m_notifyCount.store(m_notifyCount.load() + 1);
                m_eventVar.notify_one();
            }
            else
            {
                m_notifyCount.store(0);
                auto currentEvent = m_eventList.front();
                m_eventList.pop_front();
                return currentEvent;
            }
        }
        return e;
    }
};


ObjectDetector::Interface::Interface() :
    d { new ObjectDetector::Interface::InterfacePrivate() }
{

}

ObjectDetector::Interface::~Interface()
{

}

void ObjectDetector::Interface::setOutputFile(const std::string &ofPath)
{
    d->m_ofpath = ofPath;
}

void ObjectDetector::Interface::init()
{
    if (status() != ObjectDetector::ProgramStatus::INIT_ERROR)
        return; // Do not switch status in case of error messaging


    setStatus(ObjectDetector::ProgramStatus::READY);
    PRINT_SUC("Complete");
}

void ObjectDetector::Interface::start()
{
    if (status() != ObjectDetector::ProgramStatus::READY)
        return;

    // Setup for start
    PRINT_LOG("Preparing");
    setStatus(ObjectDetector::ProgramStatus::PROCESSING);


    d->m_workFut = std::async(
        [this]()
        {
            // Working loop
            PRINT_LOG("Starting");
            setStatus(ObjectDetector::ProgramStatus::IDLE);
            d->event(ObjectDetector::EventType::STARTED);
            while (status() == ObjectDetector::ProgramStatus::IDLE)
            {
                PRINT_LOG("Waiting for events");
                if (d->waitForEvent(ObjectDetector::EventType::ANY) == ObjectDetector::EventType::STOP)
                {
                    setStatus(ObjectDetector::ProgramStatus::INIT_SUCCESS);
                }
                PRINT_LOG("Got some event");
            }


            // Exited from working loop
            PRINT_LOG("Exited");
            setStatus(ObjectDetector::ProgramStatus::INIT_SUCCESS);
            d->event(ObjectDetector::EventType::STOPPED);
        }
    );
}

void ObjectDetector::Interface::stop()
{
    d->event(ObjectDetector::EventType::STOP, true);
    PRINT_LOG("Called");
}

void ObjectDetector::Interface::poll()
{
    d->m_workFut.get();
}

ObjectDetector::ProgramStatus ObjectDetector::Interface::status() const
{
    return d->m_status.load();
}

void ObjectDetector::Interface::setStatus(ObjectDetector::ProgramStatus s)
{
    d->m_status.store(s);
}
