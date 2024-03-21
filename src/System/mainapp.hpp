#ifndef COMPONENTS_MAIN_APP_H
#define COMPONENTS_MAIN_APP_H

#include <vector>

#include "module.hpp"

namespace Components
{

class MainApp
{
public:
    // Args are derived from main() function
    MainApp(int argc, char* argv[]);
    ~MainApp();

    void addModule(Module m);

    // Work with arguments passed to app
    std::string argument(std::size_t argNo);
    std::size_t argCount() const;
    std::vector<std::string> args() const;

    std::vector<Module> modules() const;
    Module getModuleByName(const std::string& _name);
    Module getModuleByType(ModuleTypes _type);
    Module getModuleByUid(ModuleUid _uid);

    // Return true if inited successfully
    bool init();

    // Return exitcode of application
    int exec();

    // Close application with code __status
    void exit();

private:
    std::vector<Module> m_moduleVect;
    std::vector<std::string> m_argsVect;
};

}

#endif // COMPONENTS_MAIN_APP_H
