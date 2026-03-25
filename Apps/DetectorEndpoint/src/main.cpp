#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/ApplicationSettings.h>

#include "endpoint/detectorendpoint.hpp"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

int main(int argc, char* argv[]) {

    // Configure directory manager
    auto& dirManager = Common::DirectoryManager::getInstance();
    dirManager.setRootPath("ROD-DetectorEndpoint");
    COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::DirectoryManager::Logs));

    // Get existing settings
    auto& appSettings = Common::ApplicationSettings::getInstance();
    appSettings.loadSettings(dirManager.getDirectory(Common::DirectoryManager::DirectoryType::Config) / "config.ini");

    const auto CONNECTION_CONFIG_SECTION_NAME = "CONNECTION_CONFIG";
    auto addressSetting = appSettings.getSetting(CONNECTION_CONFIG_SECTION_NAME, "address");
    if (!addressSetting) {
        addressSetting = appSettings.addSetting(CONNECTION_CONFIG_SECTION_NAME, "address");
    }

    auto eventPortSetting = appSettings.getSetting(CONNECTION_CONFIG_SECTION_NAME, "eventport");
    if (!eventPortSetting) {
        eventPortSetting = appSettings.addSetting(CONNECTION_CONFIG_SECTION_NAME, "eventport");
    }

    auto streamingPortSetting = appSettings.getSetting(CONNECTION_CONFIG_SECTION_NAME, "streamingport");
    if (!streamingPortSetting) {
        streamingPortSetting = appSettings.addSetting(CONNECTION_CONFIG_SECTION_NAME, "streamingport");
    }

    auto pTokenSetting = appSettings.getSetting(CONNECTION_CONFIG_SECTION_NAME, "token");
    if (!pTokenSetting) {
        pTokenSetting = appSettings.addSetting(CONNECTION_CONFIG_SECTION_NAME, "token");
    }

    appSettings.saveSettings(); // For creating empty config file

    // Address and ports
    std::string serverAddress {addressSetting->getValue().has_value() ? addressSetting->getValueString() : ""};
    auto eventPortLL = eventPortSetting->getValue().has_value() ? std::get<long long>(eventPortSetting->getValue().value()) : 0;
    auto streamingPortLL = streamingPortSetting->getValue().has_value() ? std::get<long long>(streamingPortSetting->getValue().value()) : 0;

    // Program options setup
    bpo::options_description desc;
    desc.add_options()
            ("host,-h",         bpo::value(&serverAddress),     "Address of a server in IPv4 format. In config value: address")
            ("event-port,-e",   bpo::value(&eventPortLL),       "WSS device event port. In config value: address")
            ("stream-port,-s",  bpo::value(&streamingPortLL),   "UDP streaming port. In config value: address")
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

    // Check event port
    if (eventPortLL < 0 || eventPortLL > 65535) {
        COMPLOG_ERROR("Invalid port for events:", eventPortLL);
        return -1;
    }
    uint16_t eventPort {static_cast<uint16_t>(eventPortLL)};

    // Check streaming port
    if (streamingPortLL < 0 || streamingPortLL > 65535 || streamingPortLL == eventPortLL) {
      COMPLOG_ERROR("Invalid port for streaming:", streamingPortLL);
      return -1;
    }
    uint16_t streamingUDPPort {static_cast<uint16_t>(streamingPortLL)};

    // Get token
    if (!pTokenSetting->getValue().has_value() || pTokenSetting->getValueString().size() != 64) {
        COMPLOG_ERROR("Failed to configure endpoint: no token set or it's invalid (must be 64 byte string)");
        return 1;
    }

    // Start endpoint using parameters
    DetectorEndpoint endpoint;
    endpoint.setToken(std::get<std::string>(pTokenSetting->getValue().value()));
    if (!endpoint.start(serverAddress, streamingUDPPort, eventPort)) {
        COMPLOG_ERROR("Detector endpoint not started");
        return 1;
    }
    COMPLOG_INFO("Endpoint exited");
    return 0;
}
