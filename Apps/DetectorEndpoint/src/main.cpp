#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <Components/Network/ClientUDP.h>

#include <thread>

int main(int argc, char* argv[]) {

    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(PROJECT_NAME_STRING);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    UDP::Client client;
    client.setHost("127.0.0.1", 9002);

    for (int i = 0; i < 10; ++i) {
        client.sendData("Test data");
        COMPLOG_DEBUG("Sent data");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    COMPLOG_INFO("Endpoint exited");
    return 0;
}
