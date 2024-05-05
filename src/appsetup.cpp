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
Module createCameraModule(const std::string& cameraFile)
{
    // Create camera adaptor
    auto pCamera = std::shared_ptr<Drivers::CameraDriver>(new Drivers::CameraDriver(cameraFile), std::default_delete<Drivers::CameraDriver>());

    // Setup configuration of module
    ModuleConfiguration cameraConfig;
    cameraConfig.type = ModuleTypes::MODULE_TYPE_CAMERA;
    cameraConfig.name = "Camera";
    cameraConfig.initAsync = true;
    cameraConfig.workAsync = true;
    cameraConfig.addRequiredConnectionType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR);

    // Setup camera for use
    cameraConfig.initFunction = [pCamera](Module m){

        pCamera->init();

        // Check if camera has been inited
        if (pCamera->status() != Drivers::DriverStatus::READY)
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
    cameraConfig.workFunction = [pCamera](Module m){
        m->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);

        const std::string tempPhotoPath {"temp/photoshot.png"};

        // Try to shot photos every second
        while (m->status() == ModuleStatus::MODULE_STATUS_RUNNING)
        {
            if (pCamera->status() == Drivers::DriverStatus::ERROR)
            {
                LOG_ERROR("Camera adaptor error (is camera conencted?)");
                LOG_WARNING("Camera reinit...");
                pCamera->deinit();
                m->sleep_s(5);
                pCamera->init();
                m->sleep_s(5);
                continue;
            }

            if (pCamera->shot(tempPhotoPath))
                m->sendToModuleType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR, tempPhotoPath);

            m->sleep_s(1);
        }
        return ModuleExitCode::MODULE_EXIT_CODE_SUCCESS;
    };

    // Setup stop function for camera module
    cameraConfig.stopFunction = [](Module m) {
        m->setStatus(ModuleStatus::MODULE_STATUS_STOPPING);
    };

    return ModuleClass::createModule(cameraConfig);
}



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
        int cnt = 1;
        LOG_EMPTY("------------------------------------");
        for (auto & t : pImageProc->availableTypes())
            LOG_EMPTY("%i) %s", cnt++, t.c_str());
        LOG_EMPTY("------------------------------------");

        // Say that all's good
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    // Setup request processor
    imageProcessorConfig.inputProcessor = [pImageProc](Message msg){
        std::pair<std::string, float> foundObject = pImageProc->getObject(msg->payload);

        // This output can be replaced with sending result to any other source
        LOG_DEBUG("Found [ %s ]", foundObject.first.c_str());

        // Response with deduced type
        msg->payload = foundObject.first;
        std::swap(msg->senderUid, msg->senderUid);
        return msg;
    };

    return ModuleClass::createModule(imageProcessorConfig);
}



Module createEmulatorModule()
{
    ModuleConfiguration emulatorConfig;
    emulatorConfig.type = ModuleTypes::MODULE_TYPE_TEST_MODULE;
    emulatorConfig.name = "Emulator";
    emulatorConfig.initAsync = true;
    emulatorConfig.workAsync = true;
    emulatorConfig.addRequiredConnectionType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR);


    emulatorConfig.initFunction = [](Module m){
        return ModuleStatus::MODULE_STATUS_INITED;
    };

    emulatorConfig.workFunction = [](Module m){
        m->setStatus(ModuleStatus::MODULE_STATUS_RUNNING);
        Message response;

        LOG_DEBUG("Camera emulator started");

//        const std::string basepath = "test/";
        const std::string basepath = "temp/";

//        const std::string path = basepath +"black_knight_rotates";
//        const std::string path = basepath + "black_knight_distort";
//        const std::string path = basepath + "distorts";
        const std::string path = basepath + "figures"; // Object templates

        if (!stdfs::exists(path) || !stdfs::is_directory(path))
        {
            LOG_ERROR("Invalid analyse directory: %s", path.c_str());
            return ModuleExitCode::MODULE_EXIT_CODE_ERROR;
        }

        for (const auto& dirent : stdfs::directory_iterator(path))
        {
            if (m->status() != ModuleStatus::MODULE_STATUS_RUNNING)
                break;

            if (stdfs::is_regular_file(dirent.path()))
            {
                LOG_INFO("Analysing picture: %s", dirent.path().filename().c_str());

                auto timeNow = std::chrono::high_resolution_clock::now();
                response = m->sendToModuleType(ModuleTypes::MODULE_TYPE_IMAGE_PROCESSOR, dirent.path());
                auto timeElapsed = std::chrono::high_resolution_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed - timeNow);

                LOG_DEBUG("Analyse time: %.3f s", dur.count() / 1000.0f );
                LOG_EMPTY("");
            }
        }

        LOG_DEBUG("Camera emulator exited");
        return ModuleExitCode::MODULE_EXIT_CODE_SUCCESS;
    };

    emulatorConfig.stopFunction = [](Module m){
        m->setStatus(ModuleStatus::MODULE_STATUS_STOPPING);
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
