#include "appsetup.hpp"

#include "System/module.hpp"

#include "logging.hpp"
#include <signal.h>

#include "Images/imageprocessor.hpp"

using namespace Components;
MainApp * pApp {nullptr};


// ---------------------------------------------- //
// ---------------------------------------------- //
// ------------- MODULES CREATING --------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
ModuleConfiguration createImageProcessor(const std::vector<std::string>& templateDirs)
{
    // Create image processor
    auto pImageProc = std::shared_ptr<Analyse::Processor>(new Analyse::Processor(), std::default_delete<Analyse::Processor>());

    // Setup configuration of module
    ModuleConfiguration imageProcessorConfig;
    imageProcessorConfig.type = ModuleType::MODULE_TYPE_IMAGE_PROCESSOR;
    imageProcessorConfig.name = "Image processor";
    imageProcessorConfig.initAsync = true;
    imageProcessorConfig.workAsync = true;

    // Setup init function for image processor
    imageProcessorConfig.initFunction = [pImageProc, templateDirs](PModule selfModule){

        LOG_INFO("Studying background...");
        pImageProc->studyBackground(1000);
        LOG_OPRES_SUCCESS("Studying background complete");

        // Setup all directories as source with templates
        for (auto& templateDir : templateDirs)
            pImageProc->setImageTemplateDir(templateDir);

        // Check if templates set up
        if (!pImageProc->availableTypes().size())
        {
            LOG_ERROR("No images detected in directories ppovided");
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        // Show what types found in a directory
        LOG_EMPTY("Detected objects:");
        int templateNo = 1;
        LOG_EMPTY("------------------------------------");
        for (auto & t : pImageProc->availableTypes())
            LOG_EMPTY("%i) %s", templateNo++, t.c_str());
        LOG_EMPTY("------------------------------------");

        // Say that all's good
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    // Setup request processor
    imageProcessorConfig.messageProcessingFunction = [pImageProc](PMessage msg){

        // Process image
        std::pair<std::string, float> foundObject = pImageProc->getObject(msg->payload);

        // Response with deduced type
        msg->payload = foundObject.first;

        return msg;
    };

    return imageProcessorConfig;
}



ModuleConfiguration createEmulatorModule()
{
    // Setup configuration for emulator
    ModuleConfiguration emulatorConfig;
    emulatorConfig.type = ModuleType::MODULE_TYPE_EMULATOR_1;
    emulatorConfig.name = "Emulator";
    emulatorConfig.initAsync = true;
    emulatorConfig.workAsync = true;
    emulatorConfig.addRequiredConnection(ModuleType::MODULE_TYPE_IMAGE_PROCESSOR);

    // Setup init function
    emulatorConfig.initFunction = [](PModule selfModule){
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    // Setup work (emulation for camera) function
    emulatorConfig.mainCycleFunction = [](PModule selfModule){
        selfModule->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);
        PMessage response;

        LOG_DEBUG("Camera emulator started");

        const std::string basepath = "temp/";

//        const std::string path = basepath + "photos1/object_1"; // Photos of object 1 (GUM)
//        const std::string path = basepath + "photos1/object_2"; // Photos of object 2 (PEN)

//        const std::string path = basepath + "photos2/object_1"; // Photos of object 1 (ear)
        const std::string path = basepath + "photos2/object_2"; // Photos of object 2 (flash)

        if (!stdfs::exists(path) || !stdfs::is_directory(path))
        {
            LOG_ERROR("Invalid analyse directory: %s", path.c_str());
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        for (const auto& dirent : stdfs::directory_iterator(path))
        {
//            if (selfModule->status() != ModuleStatus::MODULE_STATUS_RUNNING)
//                break;

            if (stdfs::is_regular_file(dirent.path()))
            {
                LOG_INFO("Analysing picture: %s", dirent.path().filename().c_str());

                auto analyseTimeStart = std::chrono::high_resolution_clock::now();
                response = selfModule->sendMessage(Message::create(ModuleType::MODULE_TYPE_IMAGE_PROCESSOR, MessageType::MESSAGE_TYPE_REQUEST_ADD, dirent.path()));
                auto analyseTimeEnd = std::chrono::high_resolution_clock::now();
                auto analyseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(analyseTimeEnd - analyseTimeStart);

                LOG_DEBUG("Result: [ %s ] Analyse time: [ %.3f s ]", response->payload.c_str(), analyseDuration.count() / 1000.0f );
                LOG_EMPTY("");
            }
        }

        LOG_DEBUG("Camera emulator exited");
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    return emulatorConfig;
}



// ---------------------------------------------- //
// ---------------------------------------------- //
// --------------- MODULES SETUP ---------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
void AppSetup::setupApp(MainApp &app)
{
    pApp = &app; // For signal handling

    // Check if camera provided
    if (app.argCount() < 1)
    {
        LOG_MAINAPP_MESSAGE("No camera provided (try --help)");
        kill(0, SIGINT);
        return;
    }

    // Camera argument from command line
    const std::string cameraArgumentName {"--camera="};

    // Check if asked for help
    if (app.argument(1) == "--help")
    {
        LOG_MAINAPP_MESSAGE("Usage: %s [ --help | %s/dev/exampleCamera1 ] [ templateFilesDirectory(ies) ]", app.argument(0).c_str(), cameraArgumentName.c_str());
        kill(0, SIGINT);
        return;
    }

    // Setup camera
    std::string cameraFile = app.argument(1);
    cameraFile.erase(0, cameraArgumentName.size());

    // Check if template directories provided
    if (app.argCount() < 2)
    {
        LOG_MAINAPP_MESSAGE("No even 1 template directory provided (try --help)");
        kill(0, SIGINT);
        return;
    }

    // Setup array of template data (remove camera and zero args)
    auto args = app.args();
    args.erase(args.begin(), args.begin() + 2); // Remove 0 and camera (1) arg

    system("mkdir temp &> /dev/null"); // Create temporary dir for camera output

    // Create app modules
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
