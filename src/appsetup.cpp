#include "appsetup.hpp"

#include "System/module.hpp"

#include "logging.hpp"
#include <signal.h>

using namespace Components;
MainApp * pApp {nullptr};


// ---------------------------------------------- //
// ---------------------------------------------- //
// ------------- MODULES CREATING --------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
Module createImageProcessor()
{
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
}




// ---------------------------------------------- //
// ---------------------------------------------- //
// --------------- MODULES SETUP ---------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
void AppSetup::setupApp(MainApp &app)
{
    pApp = &app; // For signal handling
}













void signalHandler(int signo)
{
    if (signo == SIGINT)
    {
        LOG_WARNING("Interrupt signal got. Stopping app...");
        if (pApp != nullptr)
        {
            pApp->exit();
            LOG_OPRES_SUCCESS("App stopped");
        }
        else
        {
            LOG_OPRES_ERROR("Application not inited!");
            exit(-1);
        }
    }

    if (signo == SIGTERM)
    {
        LOG_WARNING("App terminated!");
        exit(1);
    }
    LOG_WARNING("Unknown signal code from system got (%i)", signo);
}

void AppSetup::independentSetup()
{
    // Setup signals for OS
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    LOG_INFO("Setup complete");
}
