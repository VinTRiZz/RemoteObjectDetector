#include <iostream>
#include <signal.h>

#include "interface.h"

using namespace std;

ObjectDetector::Interface * pApp {nullptr};

#define PRINT_LOG(what) std::cout << "[LOG CHANNEL] " << what << std::endl
#define PRINT_SUC(what) std::cout << "[LOG CHANNEL] [\033[32m OK! \033[0m] " << what << std::endl
#define PRINT_ERR(what) std::cout << "[LOG CHANNEL] [\033[31mERROR\033[0m] " << what << std::endl

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
    PRINT_LOG("[OS SIGNAL] Handled");
}

int main()
{
    ObjectDetector::Interface app;
    pApp = &app;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    PRINT_LOG("[Signals set]");

    app.init();

    if (!STATUS_CONTAINS(app.status(), ObjectDetector::ProgramStatus::READY))
    {
        PRINT_ERR("Init code: " << static_cast<int>(app.status()));
        return 1;
    }

    app.start();

    PRINT_LOG("Program complete");
    return 0;
}

