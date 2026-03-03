#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>

#include "endpoint/detectorendpoint.hpp"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath("ROD-DetectorEndpoint");
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Common setings
    std::string serverAddress {"127.0.0.1"};
    uint16_t wsPort {9001};
    uint16_t streamingUDPPort {9003};

    bpo::options_description desc;
    desc.add_options()
            ("host,-h",         bpo::value(&serverAddress),     "Address of a server in IPv4 format")
            ("event-port,-e",   bpo::value(&wsPort),            "WSS device event port")
            ("stream-port,-s",  bpo::value(&streamingUDPPort),  "UDP streaming port")
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

    DetectorEndpoint endpoint(serverAddress, streamingUDPPort, wsPort);
    endpoint.start();
    COMPLOG_INFO("Endpoint exited");
    return 0;
}
