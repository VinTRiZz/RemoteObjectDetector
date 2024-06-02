#ifndef APPSETUP_HPP
#define APPSETUP_HPP

#include "System/mainapp.hpp"

namespace AppSetup
{

// Setup everything not depending on app itself
void independentSetup();

// Setup MainApp class
void setupApp(Components::MainApp& app);

}

#endif // APPSETUP_HPP
