#include "appsetup.hpp"

#include "System/module.hpp"

#include "logging.hpp"
#include <signal.h>

#include "Images/imageprocessor.hpp"
#include "Images/cameradriver.hpp"

using namespace Components;
MainApp * pApp {nullptr};


// ---------------------------------------------- //
// ---------------------------------------------- //
// ------------- MODULES CREATING --------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
Module createImageProcessor(const std::vector<std::string>& templateDirs)
{
    // Create image processor
    auto pImageProc = std::shared_ptr<Analyse::Processor>(new Analyse::Processor(), std::default_delete<Analyse::Processor>());

    // Setup configuration of module
    ModuleConfiguration imageProcessorConfig;
    imageProcessorConfig.type = ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR;
    imageProcessorConfig.name = "Image processor";
    imageProcessorConfig.initAsync = true;
    imageProcessorConfig.workAsync = true;

    // Setup init function for image processor
    imageProcessorConfig.initFunction = [pImageProc, templateDirs](Module m){

        for (auto& templateDir : templateDirs)
            pImageProc->setImageTemplateDir(templateDir);

        // Show what types found in a directory
        LOG_INFO("Found types:");
        int cnt = 1;
        for (auto & t : pImageProc->availableTypes())
            LOG_EMPTY("%i) %s", cnt++, t.c_str());
        LOG_EMPTY("------------------------------------");

        return ModuleStatus::MODULE_STATUS_INITED;
    };

    imageProcessorConfig.inputProcessor = [pImageProc](Message msg){

        LOG_DEBUG("Testing work of image processor");
        auto objects = pImageProc->getObjects(msg->payload, 0.4);

        float mustBe = 0;
        std::pair<std::string, float> foundObject;
        for (auto& obj : objects)
        {
            if (obj.second > mustBe)
                foundObject = obj;

            LOG_OPRES_SUCCESS("Type [ %s ] match percent: [ %f ]", obj.first.c_str(), obj.second);
        }
        LOG_INFO("On a picture object with type: \033[32m%s\033[0m", foundObject.first.c_str());
        LOG_DEBUG("Testing work of image processor COMPLETE");

        return msg;
    };

    return ModuleClass::createModule(imageProcessorConfig);
}


Module createCameraModule(const std::string& cameraFile)
{
    ModuleConfiguration cameraConfig;
    cameraConfig.name = "Camera";
    cameraConfig.initAsync = true;
    cameraConfig.workAsync = true;
    cameraConfig.addRequiredConnectionType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR);


    cameraConfig.initFunction = [](Module m){
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    cameraConfig.workFunction = [](Module m){
        m->sendToModuleType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR, "templates/white-king.png");
        return ModuleExitCode::MODULE_EXIT_CODE_SUCCESS;
    };

    return ModuleClass::createModule(cameraConfig);
}




// ---------------------------------------------- //
// ---------------------------------------------- //
// --------------- MODULES SETUP ---------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
void AppSetup::setupApp(MainApp &app)
{
    pApp = &app; // For signal handling

    const std::string cameraArgumentName {"--camera="};

    if (app.argCount() > 1)
    {
        if (app.argument(1) == "--help")
        {
            LOG_MAINAPP_MESSAGE("Usage: %s [ --help | %s/dev/exampleCamera1 ] [ templateFilesDirectory(ies) ]", cameraArgumentName.c_str());
            kill(0, SIGINT);
        }
    }
    else
    {
        LOG_MAINAPP_MESSAGE("No camera provided (try --help)");
        kill(0, SIGINT);
    }


    auto args = app.args();
    if (args.size() > 2)
    {
        args.erase(args.begin(), args.begin() + 2); // Remove 0 arg
    }
    else
    {
        LOG_MAINAPP_MESSAGE("No even 1 template directory provided (try --help)");
        kill(0, SIGINT);
    }

    std::string cameraFile = app.argument(1);
    cameraFile.erase(0, cameraArgumentName.size());

    app.addModule(createCameraModule(cameraFile));
    app.addModule(createImageProcessor(args));
}













void signalHandler(int signo)
{
    if (signo == SIGINT)
    {
        LOG_MAINAPP_MESSAGE("Exiting...");
        if (pApp != nullptr)
        {
            pApp->exit();
            LOG_OPRES_SUCCESS("App exited");
            exit(0);
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
    LOG_MAINAPP_MESSAGE("Independent setup complete");
}
