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
        float mustBe = 0;
        std::pair<std::string, float> foundObject = pImageProc->getObjects(msg->payload, 0.9, true);

        LOG_EMPTY("It's [ %s ] match percent: [ %f ]", foundObject.first.c_str(), foundObject.second);

        // Response with deduced type
        msg->payload = foundObject.first;
        std::swap(msg->senderUid, msg->senderUid);
        return msg;
    };

    return ModuleClass::createModule(imageProcessorConfig);
}


Module createCameraModule(const std::string& cameraFile)
{
    auto pCamera = std::shared_ptr<Drivers::CameraDriver>(new Drivers::CameraDriver(cameraFile), std::default_delete<Drivers::CameraDriver>());
    auto pDoneSignal = std::make_shared<bool>(false);

    ModuleConfiguration cameraConfig;
    cameraConfig.type = ModuleTypes::MODULE_TYPE_CAMERA;
    cameraConfig.name = "Camera";
    cameraConfig.initAsync = true;
    cameraConfig.workAsync = true;
    cameraConfig.addRequiredConnectionType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR);


    cameraConfig.initFunction = [pCamera](Module m){

        pCamera->init();

        if (pCamera->status() != Drivers::DriverStatus::READY)
        {
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        return ModuleStatus::MODULE_STATUS_INITED;
    };

    cameraConfig.workFunction = [pCamera, pDoneSignal](Module m){
        m->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);

        const std::string tempPhotoPath {"photoshot.png"};
        while (!*pDoneSignal)
        {
            m->sleep_s(1);
            if (!pCamera->shot(tempPhotoPath))
            {
                LOG_OPRES_ERROR("Camera did not shot");
                continue;
            }

            LOG_OPRES_SUCCESS("Photo got, processing...");
            m->sendToModuleType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR, tempPhotoPath);
        }
        return ModuleExitCode::MODULE_EXIT_CODE_SUCCESS;
    };

    cameraConfig.stopFunction = [pDoneSignal](Module m) {
        *pDoneSignal = true;
        m->setStatus(ModuleStatus::MODULE_STATUS_STOPPING);
    };

    return ModuleClass::createModule(cameraConfig);
}


Module createEmulatorModule()
{
    auto pDoneSignal = std::make_shared<bool>(false);

    ModuleConfiguration emulatorConfig;
    emulatorConfig.type = ModuleTypes::MODULE_TYPE_TEST_MODULE;
    emulatorConfig.name = "Emulator";
    emulatorConfig.initAsync = true;
    emulatorConfig.workAsync = true;
    emulatorConfig.addRequiredConnectionType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR);


    emulatorConfig.initFunction = [](Module m){
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    emulatorConfig.workFunction = [pDoneSignal](Module m){
        m->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);
        Message response;

//        const std::string path = "black_knight_rotates";
        const std::string path = "black_knight_distort";
//        const std::string path = "distorts";
//        const std::string path = "figures"; // Object templates

        for (const auto& dirent : stdfs::directory_iterator(path))
        {
            if (stdfs::is_regular_file(dirent.path()))
            {
                LOG_DEBUG("Analysing picture: %s", dirent.path().filename().c_str());
                response = m->sendToModuleType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR, dirent.path());
                LOG_DEBUG("Must be [ %s ]", response->payload.c_str());
                LOG_EMPTY("");
            }
        }

        m->setStatus(ModuleStatus::MODULE_STATUS_STOPPING);
        return ModuleExitCode::MODULE_EXIT_CODE_SUCCESS;
    };

    return ModuleClass::createModule(emulatorConfig);
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

//    app.addModule(createCameraModule(cameraFile));
    app.addModule(createImageProcessor(args));
    app.addModule(createEmulatorModule());
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
