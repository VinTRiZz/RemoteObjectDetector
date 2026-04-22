#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <boost/program_options.hpp>

#include "endpoints/serverendpoint.hpp"
#include "common/servercommon.hpp"

#include <filesystem>
#include <iostream>

namespace bpo = boost::program_options;

#define APP_EXITCODE_OK                   0
#define APP_EXITCODE_CONFIGURATION_ERROR  1
#define APP_EXITCODE_FAILURE              2
#define APP_EXITCODE_EXCEPTION            3
#define APP_EXITCODE_UNKNOWN_EXCEPTION    4

int main(int argc, char* argv[]) {

    // Common setings
    uint16_t httpAPIPort {9001};
    uint16_t wsPort {9002};
    uint16_t streamingUDPPort {9003};
    std::string updatesDir {"."};

    bpo::options_description desc;
    desc.add_options()
            ("event-port,-e",   bpo::value(&wsPort),            "WSS device event port")
            ("api-port,-a",     bpo::value(&httpAPIPort),       "HTTP API port")
            ("stream-port,-s",  bpo::value(&streamingUDPPort),  "UDP streaming port")
            ("data,-d",         bpo::value(&updatesDir),        "Path to directory to use for saving server data (current dir by default)")
            ;

    // Harvest settings
    try {
        auto options = bpo::parse_command_line(argc, argv, desc, bpo::command_line_style::unix_style);
        bpo::variables_map vm;
        bpo::store(options, vm);
        bpo::notify(vm);
    } catch (bpo::error& er) {
        std::cerr << er.what() << std::endl;
        desc.print(std::cerr);
        std::cerr << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Check-up
    if (!std::filesystem::exists(updatesDir)) {
        std::cerr << "Invalid updates directory path: " << updatesDir << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Setup root of application and logging
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(CCR_APP_TARGET_NAME);
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Configure updates dir
    auto softVersionsDir = dirManager.getDirectory(Common::DirectoryManager::Data) / "versions";
    std::filesystem::create_directory(softVersionsDir);
    dirManager.registerDirectory(ServerCommon::DIRTYPE_SOFT_VERSIONS, softVersionsDir);

    // Start server
    ServerEndpoint server(dirManager.getDirectory(Common::DirectoryManager::DirectoryType::Data) / "local.db");
#ifdef DEBUG_BUILD_MODE
    server.start(wsPort, httpAPIPort, streamingUDPPort); // For exception handling
#else
    try {
        server.start(wsPort, httpAPIPort, streamingUDPPort);
    } catch (const std::exception& ex) {
        try {
            server.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: EXCEPTION: " << ex.what() << std::endl;
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        try {
            server.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: UNKNOWN EXCEPTION" << std::endl;
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
#endif // Release build
    return APP_EXITCODE_OK;
}
