#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

int main(int argc, char* argv[]) {

    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(PROJECT_NAME_STRING);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    COMPLOG_INFO("Endpoint exited");
    return 0;
}
