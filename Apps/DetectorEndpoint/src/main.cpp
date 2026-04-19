#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/ApplicationSettings.h>

#include "endpoint/detectorendpoint.hpp"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

#define APP_EXITCODE_OK                   0
#define APP_EXITCODE_CONFIGURATION_ERROR  1
#define APP_EXITCODE_FAILURE              2
#define APP_EXITCODE_EXCEPTION            3
#define APP_EXITCODE_UNKNOWN_EXCEPTION    4

int main(int argc, char* argv[]) {
    // Address and ports
    std::string serverAddress {};
    long long eventPortLL     {};
    long long streamingPortLL {};

    // Program options setup
    bpo::options_description desc;
    desc.add_options()
            ("host,-h",         bpo::value(&serverAddress),     "Address of a server in IPv4 format. In config value: address")
            ("event-port,-e",   bpo::value(&eventPortLL),       "WSS device event port. In config value: address")
            ("stream-port,-s",  bpo::value(&streamingPortLL),   "UDP streaming port. In config value: address")
            ("debug,-d",                                        "Start in debug mode (send test data insead of camera)")
            ;

    // Harvest settings
    bpo::variables_map vm;
    try {
        auto options = bpo::parse_command_line(argc, argv, desc, bpo::command_line_style::unix_style);
        bpo::store(options, vm);
        bpo::notify(vm);
    } catch (bpo::error& er) {
        std::cerr << er.what() << std::endl;
        desc.print(std::cerr);
        std::cerr << std::endl;
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

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
    auto pDevIdSetting = appSettings.getSetting(CONNECTION_CONFIG_SECTION_NAME, "device_id");
    if (!pDevIdSetting) {
        pDevIdSetting = appSettings.addSetting(CONNECTION_CONFIG_SECTION_NAME, "device_id");
    }
    appSettings.saveSettings(); // For creating empty config file

    // Setup settings
    serverAddress   = (serverAddress.empty() && addressSetting->getValue().has_value()) ? addressSetting->getValueString() : serverAddress;
    eventPortLL     = (eventPortLL == 0 && eventPortSetting->getValue().has_value()) ? std::get<long long>(eventPortSetting->getValue().value()) : eventPortLL;
    streamingPortLL = (streamingPortLL == 0 && streamingPortSetting->getValue().has_value()) ? std::get<long long>(streamingPortSetting->getValue().value()) : streamingPortLL;

    // Check event port
    if (eventPortLL < 0 || eventPortLL > 65535) {
        COMPLOG_ERROR("Invalid port for events:", eventPortLL);
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    uint16_t eventPort {static_cast<uint16_t>(eventPortLL)};

    // Check streaming port
    if (streamingPortLL < 0 || streamingPortLL > 65535 || streamingPortLL == eventPortLL) {
      COMPLOG_ERROR("Invalid port for streaming:", streamingPortLL);
      return APP_EXITCODE_CONFIGURATION_ERROR;
    }
    uint16_t streamingUDPPort {static_cast<uint16_t>(streamingPortLL)};

    // Get device id
    if (!pDevIdSetting->getValue().has_value() || !std::get<long long>(pDevIdSetting->getValue().value())) {
        COMPLOG_ERROR("Failed to configure endpoint: no token set or it's invalid (must be integer >=1)");
        return APP_EXITCODE_CONFIGURATION_ERROR;
    }

    // Start endpoint using parameters
    DetectorEndpoint endpoint;
    endpoint.setDeviceId(std::get<long long>(pDevIdSetting->getValue().value()));
    endpoint.setDebugMode(vm.count("debug") != 0);
    try {
        if (!endpoint.start(serverAddress, streamingUDPPort, eventPort)) {
            COMPLOG_ERROR("Detector endpoint not started");
            return APP_EXITCODE_FAILURE;
        }
    } catch (const std::exception& ex) {
        try {
            endpoint.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: EXCEPTION: " << ex.what() << std::endl;
        return APP_EXITCODE_EXCEPTION;
    } catch (...) {
        try {
            endpoint.stop();
        } catch (...) {
            std::cerr << "CRITICAL: FAILED TO STOP APP AFTER FAILURE" << std::endl;
            return APP_EXITCODE_FAILURE;
        }
        std::cerr << "CRITICAL: UNKNOWN EXCEPTION" << std::endl;
        return APP_EXITCODE_UNKNOWN_EXCEPTION;
    }
    return APP_EXITCODE_OK;
}
