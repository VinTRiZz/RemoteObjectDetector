#pragma once

/**
 * @file File with constants to work with main server HTTP API
 */

#include <string>

namespace Protocol
{

namespace API
{

// Available values in API targets (if possible)
namespace PARAMATER_VALUES
{
const auto POWER_OFF    {"shutdown"};
const auto POWER_REBOOT {"reboot"};
}


// Server HTTP API target bases
const std::string   API_VERSION         {"v1"};
const std::string   SERVER_BASE         {"/api/" + API_VERSION + "/server"};
const auto          SERVER_STATUS_BASE  {SERVER_BASE + "/status"};
const auto          SERVER_POWER_BASE   {SERVER_BASE + "/power"};
const std::string   DETECTOR_BASE       {"/api/" + API_VERSION + "/detector"};
const auto          DETECTOR_APP_BASE   {DETECTOR_BASE + "/soft"};


// API in Qt side
namespace QT
{
const auto SERVER_STATUS    {SERVER_STATUS_BASE};
const auto SERVER_POWER     {SERVER_POWER_BASE + "/%1"};

const auto DETECTOR_APP_VERSION_GET_ALL {DETECTOR_APP_BASE + "/software/versions"};
const auto DETECTOR_APP_VERSION_GET     {DETECTOR_APP_BASE + "/software/%1"};
const auto DETECTOR_APP_VERSION_ADD     {DETECTOR_APP_BASE + "/software/versions"};
const auto DETECTOR_APP_VERSION_SET     {DETECTOR_APP_BASE + "/software/%1?version=%2"};
const auto DETECTOR_APP_VERSION_REM     {DETECTOR_APP_BASE + "/software/versions/%1"};

const auto DETECTOR_STATUS              {DETECTOR_BASE + "/detector/%1/status"};
const auto DETECTOR_POWER               {DETECTOR_BASE + "/detector/%1/power/%2"};
const auto DETECTOR_TOGGLE_STREAMING    {DETECTOR_BASE + "/detector/%1/streaming?enabled=%2"};
}


// API in Drogon side
namespace DROGON
{
const auto SERVER_STATUS    {SERVER_STATUS_BASE};
const auto SERVER_POWER     {SERVER_POWER_BASE + "/{action_type}"};

const auto DETECTOR_APP_VERSION_GET_ALL {DETECTOR_APP_BASE + "/software/versions"};
const auto DETECTOR_APP_VERSION_GET     {DETECTOR_APP_BASE + "/software/{dev_uuid}"};
const auto DETECTOR_APP_VERSION_ADD     {DETECTOR_APP_BASE + "/software/versions"};
const auto DETECTOR_APP_VERSION_SET     {DETECTOR_APP_BASE + "/software/{dev_uuid}?version={version_uuid}"};
const auto DETECTOR_APP_VERSION_REM     {DETECTOR_APP_BASE + "/software/versions/{version_uuid}"};

const auto DETECTOR_STATUS              {DETECTOR_BASE + "/detector/{dev_uuid}/status"};
const auto DETECTOR_POWER               {DETECTOR_BASE + "/detector/{dev_uuid}/power/{power_action}"};
const auto DETECTOR_TOGGLE_STREAMING    {DETECTOR_BASE + "/detector/{dev_uuid}/streaming?enabled={is_enabled}"};
}

}

// Available headers in HTTP packets
namespace Headers
{

const auto VERSION_ADD_NAME     {"x-version-name"};     // "My honest version" in hex (for compatibility)
const auto VERSION_ADD_NUMBER   {"x-version-number"};   // 1.0.0, 2.0.1, etc.
const auto VERSION_ADD_TYPE     {"x-version-type"};     // Stable, unstable, test, etc.


}


}