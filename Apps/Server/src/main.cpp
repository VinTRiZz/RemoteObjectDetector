#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include <boost/program_options.hpp>

#include "endpoint/serverendpoint.hpp"

#include <filesystem>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {

    // Common setings
    uint16_t wsPort {9001};
    uint16_t httpAPIPort {9002};
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
        return 1;
    }

    // Check-up
    if (!std::filesystem::exists(updatesDir)) {
        std::cerr << "Invalid updates directory path: " << updatesDir << std::endl;
        return 1;
    }

    // Setup root of application and logging
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath(PROJECT_NAME_STRING);
    Logging::LoggingMaster::getInstance(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Configure updates dir
    auto softVersionsDir = dirManager.getDirectory(Common::DirectoryManager::Data) / "versions";
    std::filesystem::create_directory(softVersionsDir);
    dirManager.registerDirectory(ServerCommon::DIRTYPE_SOFT_VERSIONS, softVersionsDir);

    // Start server
    ServerEndpoint server(dirManager.getDirectory(Common::DirectoryManager::DirectoryType::Data) / "local.db");
    server.start(wsPort, httpAPIPort, streamingUDPPort);
    return 0;
}
