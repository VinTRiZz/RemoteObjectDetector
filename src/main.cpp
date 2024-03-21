#include <iostream>
#include <signal.h>

#include "appsetup.hpp"

using namespace std;
int main(int argc, char* argv[])
{
    AppSetup::independentSetup();

    Components::MainApp app(argc, argv);

    AppSetup::setupApp(app);

    app.init();

    return app.exec();
}
