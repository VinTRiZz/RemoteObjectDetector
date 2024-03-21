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

    // Return true if inited successfully
    bool init();

    // Return exitcode of application
    int exec();

    // Close application with code __status
    void exit();

private:
    std::vector<Module> m_moduleVect;
};

}

#endif // COMPONENTS_MAIN_APP_H
