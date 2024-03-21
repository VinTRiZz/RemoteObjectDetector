#include "System/mainapp.hpp"
#include "logging.hpp"

Components::MainApp::MainApp(int argc, char *argv[])
{
    if (argc > 0)
    {
        LOG_MAINAPP_MESSAGE("Program started with arguments:");

        LOG_EMPTY("----------------------------------------------------------------");
        for (int i = 0; i < argc; i++)
        {
            m_argsVect.push_back(argv[i]);
            LOG_EMPTY(argv[i]);
        }

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
    LOG_MAINAPP_MESSAGE("Added module: " + m->name());
    m_moduleVect.push_back(m);
}

std::size_t Components::MainApp::argCount() const
{
    return m_argsVect.size();
}

std::vector<std::string> Components::MainApp::args() const
{
    return m_argsVect;
}

std::vector<Components::Module> Components::MainApp::modules() const
{
    return m_moduleVect;
}

Components::Module Components::MainApp::getModuleByName(const std::string &_name)
{
    for (auto m : m_moduleVect)
    {
        if (m->name() == _name)
            return m;
    }
    return Module();
}

Components::Module Components::MainApp::getModuleByType(Components::ModuleTypes _type)
{
    for (auto m : m_moduleVect)
    {
        if (m->type() == _type)
            return m;
    }
    return Module();
}

Components::Module Components::MainApp::getModuleByUid(Components::ModuleUid _uid)
{
    for (auto m : m_moduleVect)
    {
        if (m->uid() == _uid)
            return m;
    }
    return Module();
}

std::string Components::MainApp::argument(std::size_t argNo)
{
    if (m_argsVect.size() > argNo)
    {
        return m_argsVect[argNo];
    }
    return "";
}

bool Components::MainApp::init()
{
    LOG_MAINAPP_MESSAGE("Initialisation started");

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
            LOG_OPRES_SUCCESS("Module inited: " + module->name() + " (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
            continue;
        }

        LOG_OPRES_ERROR("Module: " + module->name() + " init error (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
    }

    for (auto& moduleInitPair : asyncInitResults)
    {
        if (moduleInitPair.second.valid())
        {
            moduleInitPair.first->setStatus(moduleInitPair.second.get());
            if (moduleInitPair.first->status() == ModuleStatus::MODULE_STATUS_INITED)
            {
                LOG_OPRES_SUCCESS("Module inited: " + moduleInitPair.first->name() + " (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
            }
            else
            {
                LOG_OPRES_ERROR("Module: " + moduleInitPair.first->name() + " init error (" + std::to_string(currentModuleNo++) + " / " + std::to_string(m_moduleVect.size()) + ")");
            }
        }
    }

    // Connect all to all. No big need to optimise (one time fast proceed)
    for (auto module : m_moduleVect)
    {
        LOG_INFO(std::string("Connecting module ") + module->name() + " (" + std::to_string(module->uid()) + ")");

        for (auto pCon : m_moduleVect)
        {
            // Skip self
            if (pCon->uid() == module->uid())
                continue;

            // Connect to needed modules by ids
            for (auto con : module->m_config.requiredConnections)
            {
                if (pCon->uid() == con)
                {
                    LOG_INFO(std::string("-------> Connected to module ") + pCon->name() + " (" + std::to_string(pCon->uid()) + ")");
                    module->m_connections.push_back(pCon);
                    break;
                }
            }

            // Connect to needed modules by types
            for (auto con : module->m_config.requiredConnectionTypes)
            {
                if (pCon->type() == con)
                {
                    LOG_INFO(std::string("-------> Connected to module ") + pCon->name() + " (" + std::to_string(pCon->uid()) + ")");
                    module->m_connections.push_back(pCon);
                    break;
                }
            }
        }
    }

    LOG_MAINAPP_MESSAGE("Init complete");
    return (initedModuleCount == m_moduleVect.size());
}

int Components::MainApp::exec()
{
    LOG_MAINAPP_MESSAGE("Starting modules");
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
            LOG_OPRES_SUCCESS(std::string("Module ") + module->name() + " started");
        }
        else
        {
            LOG_OPRES_ERROR(std::string("Module ") + module->name() + " not started");
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

    LOG_MAINAPP_MESSAGE("App exit normal");
    this->exit();
    return 0;
}

void Components::MainApp::exit()
{
    for (auto module : m_moduleVect)
    {
        if (module->status() == ModuleStatus::MODULE_STATUS_RUNNING)
            module->stop();
    }
}
