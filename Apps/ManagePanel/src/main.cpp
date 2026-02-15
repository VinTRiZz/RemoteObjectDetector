#include <Components/Logger/Logger.h>

#include "gui/mainwindow.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {

    int res = -1;
    {
        QApplication a(argc, argv);

        a.setApplicationName(PROJECT_NAME_STRING);
        a.setApplicationDisplayName(PROJECT_NAME_STRING);
        a.setApplicationVersion(PROJECT_VERSION_STRING);

        MainWindow w;
        w.show();
        res = a.exec();
    }

    COMPLOG_INFO("Manage panel exited");
    return res;
}
