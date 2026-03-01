#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include "gui/mainwindow.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {

    int res = -1;
    {
        QApplication a(argc, argv);

        auto& dirManager = Common::DirectoryManager::getInstance();
        dirManager.setRootPath(PROJECT_NAME_STRING);

        COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

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
