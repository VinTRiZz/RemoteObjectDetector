#include "interface.h"

// Components
#include "imageprocessor.h"
#include "imageconverter.h"
#include "cameradriver.h"

// Event work
#include <condition_variable>
#include <mutex>
#include <future>
#include <list>

// Terminal data output
#include <iostream>

#define PRINT_LOG(what) std::cout << "[ " << __FUNCTION__ << " ] " << what << std::endl
#define PRINT_SUC(what) std::cout << "[\033[32m OK! \033[0m] " << what << std::endl
#define PRINT_ERR(what) std::cout << "[\033[31mERROR\033[0m] [ " << __PRETTY_FUNCTION__ << " ] " << what << std::endl

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

    std::condition_variable m_eventVar;     // Used to wait for events
    std::mutex m_eventMutex;                // Used to block event-signal-needed variables
    std::list<EventType> m_eventList;       // List of events to process
    size_t m_notifyCount {0};               // Used to avoid infinity signals
    std::future<void> m_workFut;            // Used for program working cycle


    ImageAnalyse::Processor m_imgProcessor; // Used to process images got from camera
    InputProcessing::CameraDriver m_camera; // Used to take pictures to process later


    // Sends signal on every event got
    void event(EventType e, bool maxPriority = false)
    {
        std::unique_lock<std::mutex> lock(m_eventMutex);

        // Add event as first to proceed if it has max priority
        if (maxPriority)
            m_eventList.push_front(e);
        else
            m_eventList.push_back(e);

        m_eventVar.notify_one(); // Start notify chain
    }

    // Waits for event until got needed
    ObjectDetector::EventType waitForEvent(EventType e)
    {
        while (m_status.load() == ObjectDetector::ProgramStatus::IDLE)
        {
            std::unique_lock<std::mutex> lock(m_eventMutex);
            m_eventVar.wait(lock);

            if ((e != ObjectDetector::EventType::ANY) && (e != m_eventList.front()) && (m_notifyCount < m_eventList.size()))
            {
                // Remove dead event (that are could not be proceed by any waiting subject)
                m_notifyCount++;
                m_eventVar.notify_one();
            }
            else
            {
                // Exit function in case of right event got and erase it from list
                m_notifyCount = 0;
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

void ObjectDetector::Interface::setImageProcessorConfigDir(const std::string &cfgPath)
{
    d->m_imgProcessor.setConfigDirPath(cfgPath);
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
    PRINT_SUC("Init complete");
}

void ObjectDetector::Interface::start()
{
    if (!STATUS_CONTAINS(status(), ObjectDetector::ProgramStatus::READY))
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
                // Exit if STOP event got
                switch ( d->waitForEvent(ObjectDetector::EventType::ANY) )
                {
                case ObjectDetector::EventType::STOP:
                    setStatus(ObjectDetector::ProgramStatus::INIT_SUCCESS);
                    break;

                case ObjectDetector::SHOT:
                    break;
                }
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
    PRINT_LOG("Called");
    d->event(ObjectDetector::EventType::STOP, true);
}

void ObjectDetector::Interface::poll()
{
    d->m_workFut.get();
}

ObjectDetector::ProgramStatus ObjectDetector::Interface::status() const
{
    return d->m_status;
}

void ObjectDetector::Interface::setStatus(ObjectDetector::ProgramStatus s)
{
    d->m_status = s;
}
