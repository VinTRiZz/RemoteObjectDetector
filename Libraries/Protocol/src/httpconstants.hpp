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
const std::string   SERVER_BASE         {"/api/server"};
const auto          SERVER_STATUS_BASE  {SERVER_BASE + "/status"};
const auto          SERVER_POWER_BASE   {SERVER_BASE + "/power"};
const std::string   DETECTOR_BASE       {"/api/detector"};
const auto          DETECTOR_APP_BASE   {DETECTOR_BASE + "/soft"};


// API in Qt side
namespace QT
{
const auto SERVER_STATUS    {SERVER_STATUS_BASE};
const auto SERVER_POWER     {SERVER_POWER_BASE + "?action=%1"};

const auto DETECTOR_APP_VERSION_GET {DETECTOR_APP_BASE + "/installed?dev=%1"};
const auto DETECTOR_APP_VERSION_ADD {DETECTOR_APP_BASE + "/register?name=%1"};
const auto DETECTOR_APP_VERSION_SET {DETECTOR_APP_BASE + "/set?dev=%1&version=%2"};
const auto DETECTOR_APP_VERSION_REM {DETECTOR_APP_BASE + "?version={1}"};

const auto DETECTOR_STATUS              {DETECTOR_BASE + "/%1/status"};
const auto DETECTOR_POWER               {DETECTOR_BASE + "/%1/power/%2"};
const auto DETECTOR_TOGGLE_STREAMING    {DETECTOR_BASE + "/%1/streaming/%2"};
}


// API in Drogon side
namespace DROGON
{
const auto SERVER_STATUS    {SERVER_STATUS_BASE};
const auto SERVER_POWER     {SERVER_POWER_BASE + "?action={1}"};

const auto DETECTOR_APP_VERSION_GET   {DETECTOR_APP_BASE + "/{1}"};
const auto DETECTOR_APP_VERSION_ADD   {DETECTOR_APP_BASE + "/register/{1}?name={2}"};
const auto DETECTOR_APP_VERSION_SET   {DETECTOR_APP_BASE + "/{1}/{2}"};
const auto DETECTOR_APP_VERSION_REM   {DETECTOR_APP_BASE + "/{1}"};

const auto DETECTOR_STATUS              {DETECTOR_BASE + "/{1}/status"};
const auto DETECTOR_POWER               {DETECTOR_BASE + "/{1}/power/{2}"};
const auto DETECTOR_TOGGLE_STREAMING    {DETECTOR_BASE + "/{1}/streaming/{2}"};
}

}

// Available headers in HTTP packets
namespace Headers
{



}


}