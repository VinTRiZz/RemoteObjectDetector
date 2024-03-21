#include "System/mainapp.hpp"

#include <loggers.hpp>
#include <iostream>

Components::MainApp::MainApp(int argc, char *argv[])
{
    if (argc > 0)
    {
        LOG_USER_INFO("Program started with arguments:");

        LOG_EMPTY("----------------------------------------------------------------");
        for (int i = 0; i < argc; i++)
            LOG_EMPTY(argv[i]);

        LOG_EMPTY("----------------------------------------------------------------");
    }

    // TODO: Use args to configure application?
}

Components::MainApp::~MainApp()
{
    this->exit();
}

void Components::MainApp::addModule(Components::Module m)
{
    LOG_EMPTY("\033[36m Added module: " + m->name() +"\033[0m");
    m_moduleVect.push_back(m);
}

bool Components::MainApp::init()
{
    LOG_EMPTY("\033[33m Initialisation started \033[0m");

    size_t initedModuleCount {0};
    size_t currentModuleNo {1};

    std::vector<std::pair<Module, std::future<ModuleStatus>>> asyncInitResults;
    for (auto module : m_moduleVect)
    {
        if (module->m_config.initAsync)
        {
            asyncInitResults.push_back( std::make_pair(module, module->initAsync()) );
            continue;
        }
        module->init();

        if (module->status() == ModuleStatus::MODULE_STATUS_INITED)
        {
            initedModuleCount++;
            LOG_EMPTY("\033[32mModule inited: " + module->name() + " (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")\033[0m");
            continue;
        }

        LOG_EMPTY("\033[31mModule: " + module->name() + " init error (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")\033[0m");
    }

    for (auto& moduleInitPair : asyncInitResults)
    {
        if (moduleInitPair.second.valid())
        {
            moduleInitPair.first->setStatus(moduleInitPair.second.get());
            if (moduleInitPair.first->status() == ModuleStatus::MODULE_STATUS_INITED)
            {
                LOG_EMPTY("\033[32mModule inited: " + moduleInitPair.first->name() + " (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")\033[0m");
            }
            else
            {
                LOG_ERROR("Module: " + moduleInitPair.first->name() + " init error (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
            }
        }
    }

    // Connect all to all. No big need to optimise (one time fast proceed)
    for (auto module : m_moduleVect)
    {
        LOG_EMPTY(std::string("Connecting module ") + module->name() + " (" + std::to_string(module->uid()) + ")");

        for (auto pCon : m_moduleVect)
        {
            // Skip self
            if (pCon->uid() == module->uid())
                continue;

            // Connect if not listed yet
            for (auto con : module->m_requiredConnections)
            {
                if (pCon->uid() == con)
                {
                    LOG_EMPTY(std::string("-------> Connected to module ") + pCon->name() + " (" + std::to_string(pCon->uid()) + ")");
                    module->m_connections.push_back(pCon);
                    break;
                }
            }

            for (auto con : module->m_requiredConnectionTypes)
            {
                if (pCon->type() == con)
                {
                    LOG_EMPTY(std::string("-------> Connected to module ") + pCon->name() + " (" + std::to_string(pCon->uid()) + ")");
                    module->m_connections.push_back(pCon);
                    break;
                }
            }
        }
        std::cout << std::endl;
    }

    LOG_EMPTY("\033[32m Init complete \033[0m");
    return (initedModuleCount == m_moduleVect.size());
}

int Components::MainApp::exec()
{
    LOG_EMPTY("\033[36mStarting app\033[0m");
    std::vector<std::future<ModuleExitCode>> moduleFutures;
    std::vector<std::shared_ptr<std::thread>> moduleThreads;

    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_INITED)
        {
            if (module->m_config.workAsync)
                moduleFutures.push_back(module->startAsync());
            else
                moduleThreads.push_back(module->startThread());
            LOG_EMPTY(std::string("Module ") + module->name() + " started");
        }
        else
        {
            LOG_ERROR(std::string("Module ") + module->name() + " not started");
        }
    }

    // Wait for async
    for (auto& fut : moduleFutures)
    {
        if (fut.valid())
            fut.get();
    }

    // Wait for threads
    for (auto pThread : moduleThreads)
    {
        if (pThread->joinable())
            pThread->join();
    }

    LOG_EMPTY("----------------------- App exit normal ----------------------");
    this->exit();
    return 0; // TODO: Add exitcodes?
}

void Components::MainApp::exit()
{
    // TODO: Work with __status
    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_RUNNING)
            module->stop();
    }
}
