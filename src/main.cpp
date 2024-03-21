#include <iostream>
#include <signal.h>

#include "interface.h"

#include "imageprocessor.h"

using namespace std;

ObjectDetector::Interface * pApp {nullptr};

#define PRINT_LOG(what) std::cout << what << std::endl

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

    // Setup signals for OS
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Setup image processor
    app.setImageTemplateDir("./imageTemplates");

    // Init system
    app.init();

    // Check if init succeed
    if (!STATUS_CONTAINS(app.status(), ObjectDetector::ProgramStatus::READY))
    {
        PRINT_LOG("Init status code: " << static_cast<int>(app.status()));
        return 1;
    }

    // Start system
    app.start();

    // TODO: Remove, test code for image processor
//    const std::string typeOfObject {"Chess Queen"};
//    const std::string objectTemplate {"../../textures/white-queen.png"};
//    const std::string fileToSearchIn {"../../textures/white-knight.png"};

//    ImageAnalyse::Processor proc;

//    PRINT_LOG("Checking if file [ " << fileToSearchIn << " ] has object with type " << typeOfObject);

//    proc.addType(typeOfObject);

//    PRINT_LOG("Existing types:");
//    PRINT_LOG("---------------");
//    int cnt = 1;
//    for (auto & t : proc.availableTypes())
//        PRINT_LOG(cnt++ << ") " << t);
//    PRINT_LOG("---------------");

//    proc.setupType(typeOfObject, objectTemplate);
//    PRINT_LOG("On a picture object with type: " << proc.processPhoto(fileToSearchIn, 0.4));
    return 0;
}
