#include <Components/Logger/Logger.h>
#include <boost/program_options.hpp>

#include "endpoint/serverendpoint.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {
    bpo::options_description desc;
    uint16_t wsPort {9001};
    uint16_t httpAPIPort {9002};
    uint16_t streamingUDPPort {9003};
    desc.add_options()
            ("event-port,-e", bpo::value(&wsPort), "WSS device event port")
            ("api-port,-a", bpo::value(&httpAPIPort),  "HTTP API port")
            ("stream-port,-s", bpo::value(&streamingUDPPort),  "UDP streaming port")
            ;

    try {
        auto options = bpo::parse_command_line(argc, argv, desc, bpo::command_line_style::unix_style);
        bpo::variables_map vm;
        bpo::store(options, vm);
        bpo::notify(vm);
    } catch (bpo::error& er) {
        std::cout << er.what() << std::endl;
        desc.print(std::cerr);
        std::cout << std::endl;
        return 1;
    }
    ServerEndpoint server;
    server.start(wsPort, httpAPIPort, streamingUDPPort);
    return 0;
}
