#include "appsetup.hpp"

#include "System/module.hpp"

#include "logging.hpp"
#include <signal.h>

#include "Images/imageprocessor.hpp"
#include "Images/cameradriver.hpp"

using namespace Components;
MainApp * pApp {nullptr};

const std::string IMAGE_TEMPLATES_DIRECTORY {"textures"};


// ---------------------------------------------- //
// ---------------------------------------------- //
// ------------- MODULES CREATING --------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
Module createImageProcessor()
{
    // Create image processor
    auto pImageProc = std::shared_ptr<ImageAnalyse::Processor>(new ImageAnalyse::Processor(), std::default_delete<ImageAnalyse::Processor>());

    // Setup configuration of module
    ModuleConfiguration imageProcessorConfig;
    imageProcessorConfig.type = ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR;
    imageProcessorConfig.name = "Image processor";
    imageProcessorConfig.initAsync = true;
    imageProcessorConfig.workAsync = true;

    imageProcessorConfig.initFunction = [pImageProc](Module m){
        pImageProc->setImageTemplateDir(IMAGE_TEMPLATES_DIRECTORY);

        LOG_INFO("Found types in a directory:");
        int cnt = 1;
        for (auto & t : pImageProc->availableTypes())
            LOG_EMPTY("%i) %s", cnt++, t.c_str());
        LOG_EMPTY("------------------------------------");


        LOG_IMPORTANT("Testing work of image processor");
        const std::string fileToSearchIn {"textures/white-knight.png"};
        LOG_INFO("On a picture object with type: \033[32m%s\033[0m", pImageProc->processPhoto(fileToSearchIn, 0.8).c_str());
        LOG_IMPORTANT("Testing work of image processor COMPLETE");

        return ModuleStatus::MODULE_STATUS_INITED;
    };

    imageProcessorConfig.inputProcessor = [pImageProc](Message msg){
        return msg;
    };

    return ModuleClass::createModule(imageProcessorConfig);
}




// ---------------------------------------------- //
// ---------------------------------------------- //
// --------------- MODULES SETUP ---------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
void AppSetup::setupApp(MainApp &app)
{
    pApp = &app; // For signal handling

    app.addModule(createImageProcessor());
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
