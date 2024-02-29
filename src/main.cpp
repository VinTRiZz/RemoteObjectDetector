#include <iostream>
#include <signal.h>

#include "interface.h"

using namespace std;

ObjectDetector::Interface * pApp {nullptr};

#define PRINT_LOG(what) std::cout << "[MAIN] " << what << std::endl

void signalHandler(int signo)
{
    // If signal called before app object inited
    if (!pApp) return;
    std::cout << std::endl; // Move pointer in terminal

    PRINT_LOG("[OS SIGNAL] Got signal");
    if (signo == SIGINT)
    {
        PRINT_LOG("[OS SIGNAL: SIGINT] Stopping...");
        pApp->stop();
    }

    if (signo == SIGTERM)
    {
        PRINT_LOG("[OS SIGNAL: SIGTERM] Terminating");
        exit(1);
    }
}

int main()
{
    ObjectDetector::Interface app;
    pApp = &app;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    app.init();

    if (!STATUS_CONTAINS(app.status(), ObjectDetector::ProgramStatus::READY))
    {
        PRINT_LOG("Init status code: " << static_cast<int>(app.status()));
        return 1;
    }

    app.start();
    return 0;
}

