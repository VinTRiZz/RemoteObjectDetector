#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/ApplicationSettings.h>

#include "gui/mainwindow.hpp"
#include <QApplication>

int main(int argc, char* argv[]) {

    int res = -1;
    auto& appSettings = Common::ApplicationSettings::getInstance();
    {
        QApplication a(argc, argv);

        auto& dirManager = Common::DirectoryManager::getInstance();
        dirManager.setRootPath("ROD-ManagementPanel");

        COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

        appSettings.loadSettings(dirManager.getDirectory(Common::DirectoryManager::Config) / "default.ini");

        a.setApplicationName(PROJECT_NAME_STRING);
        a.setApplicationDisplayName(PROJECT_NAME_STRING);
        a.setApplicationVersion(PROJECT_VERSION_STRING);

        MainWindow w;
        w.show();
        res = a.exec();
    }

    appSettings.saveSettings();
    COMPLOG_INFO("Manage panel exited");
    return res;
}
