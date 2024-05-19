#include "appsetup.hpp"

#include "System/module.hpp"

#include "logging.hpp"
#include <signal.h>

#include "Images/imageprocessor.hpp"
#include "Images/cameraadaptor.hpp"

using namespace Components;
MainApp * pApp {nullptr};


// ---------------------------------------------- //
// ---------------------------------------------- //
// ------------- MODULES CREATING --------------- //
// ---------------------------------------------- //
// ---------------------------------------------- //
PModule createCameraModule(const std::string& cameraFile)
{
    // Create camera adaptor
    auto pCamera = std::shared_ptr<Adaptors::CameraAdaptor>(new Adaptors::CameraAdaptor(cameraFile), std::default_delete<Adaptors::CameraAdaptor>());

    // Setup configuration of module
    ModuleConfiguration cameraConfig;
    cameraConfig.type = ModuleType::MODULE_TYPE_CAMERA;
    cameraConfig.name = "Camera";
    cameraConfig.initAsync = true;
    cameraConfig.workAsync = true;
    cameraConfig.addRequiredConnection(ModuleType::MODULE_TYPE_IMAGE_PROCESSOR);

    // Setup camera for use
    cameraConfig.initFunction = [pCamera](PModule selfModule){

        pCamera->init();

        // Check if camera has been inited
        if (pCamera->status() != Adaptors::AdaptorStatus::READY)
        {
            LOG_EMPTY("========================================");
            LOG_ERROR("Camera adaptor not inited. Try to install drivers for your camera. Connected USB devices:");
            system("lsusb | sed -n 's/Bus [0-9]\\{3\\} [a-zA-Z]\\{1,\\} [0-9]\\{3\\}: ID [a-z0-9:]\\{9\\}/Device:/; /Linux Foundation/d; p'");
            LOG_EMPTY("========================================");
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        return ModuleStatus::MODULE_STATUS_INITED;
    };

    // Start camera shoting photos
    cameraConfig.mainCycleFunction = [pCamera](PModule selfModule){
        selfModule->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);

        // Path for temporary photo saving
        const std::string tempPhotoPath {"temp/photoshot.png"};

        // Try to shot photos every second
        while (selfModule->status() == ModuleStatus::MODULE_STATUS_RUNNING)
        {
            // Check if camera module yet can work
            if (pCamera->status() == Adaptors::AdaptorStatus::ERROR)
            {
                LOG_ERROR("Camera adaptor error (is camera conencted?)");
                LOG_WARNING("Camera reinit...");
                pCamera->deinit();
                selfModule->sleep_s(5);
                pCamera->init();
                selfModule->sleep_s(5);
                continue;
            }

            // Get photo from camera ans save by temporary photo path
            if (pCamera->shot(tempPhotoPath))
            {
                // Process result photo
                selfModule->sendMessage(Message::create(ModuleType::MODULE_TYPE_IMAGE_PROCESSOR, MessageType::MESSAGE_TYPE_REQUEST_ADD, tempPhotoPath));
            }

            // Wait shot interval
            selfModule->sleep_s(1);
        }
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    return Module::createModule(cameraConfig);
}



PModule createImageProcessor(const std::vector<std::string>& templateDirs)
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

        // Setup all directories as source with templates
        for (auto& templateDir : templateDirs)
            pImageProc->setImageTemplateDir(templateDir);

        // Check if templates set up
        if (!pImageProc->availableTypes().size())
        {
            LOG_ERROR("No templates detected in directories");
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        // Show what types found in a directory
        LOG_INFO("Detected templates:");
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

    return Module::createModule(imageProcessorConfig);
}



PModule createEmulatorModule()
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

//        const std::string path = basepath +"black_knight_rotates";
        const std::string path = basepath + "black_knight_distort";
//        const std::string path = basepath + "distorts";
//        const std::string path = basepath + "figures"; // Object templates

        if (!stdfs::exists(path) || !stdfs::is_directory(path))
        {
            LOG_ERROR("Invalid analyse directory: %s", path.c_str());
            return ModuleStatus::MODULE_STATUS_ERROR;
        }

        for (const auto& dirent : stdfs::directory_iterator(path))
        {
            if (selfModule->status() != ModuleStatus::MODULE_STATUS_RUNNING)
                break;

            if (stdfs::is_regular_file(dirent.path()))
            {
                LOG_INFO("Analysing picture: %s", dirent.path().filename().c_str());

                auto timeNow = std::chrono::high_resolution_clock::now();
                response = selfModule->sendMessage(Message::create(ModuleType::MODULE_TYPE_IMAGE_PROCESSOR, MessageType::MESSAGE_TYPE_REQUEST_ADD, dirent.path()));
                auto timeElapsed = std::chrono::high_resolution_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed - timeNow);

                LOG_DEBUG("Result: [ %s ] Time: [ %.3f s ]", response->payload.c_str(), dur.count() / 1000.0f );
                LOG_EMPTY("");
            }
        }

        LOG_DEBUG("Camera emulator exited");
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    return Module::createModule(emulatorConfig);
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
    }

    // Camera argument from command line
    const std::string cameraArgumentName {"--camera="};

    // Check if asked for help
    if (app.argument(1) == "--help")
    {
        LOG_MAINAPP_MESSAGE("Usage: %s [ --help | %s/dev/exampleCamera1 ] [ templateFilesDirectory(ies) ]", app.argument(0).c_str(), cameraArgumentName.c_str());
        kill(0, SIGINT);
    }

    // Setup camera
    std::string cameraFile = app.argument(1);
    cameraFile.erase(0, cameraArgumentName.size());

    // Check if template directories provided
    if (app.argCount() < 2)
    {
        LOG_MAINAPP_MESSAGE("No even 1 template directory provided (try --help)");
        kill(0, SIGINT);
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
