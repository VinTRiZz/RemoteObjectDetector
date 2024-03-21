#include "logging.hpp"

#include <iostream> // Terminal printing
#include <mutex>    // To print one message per output
#include <fstream>  // For logfile
#include <cstdarg>  // Variative function logging

namespace Logging
{
static std::mutex logMutex;

const std::string DEFAULT_COLOR_CODE {"\033[0m"};
}

void Logging::log(Logging::MessageType type, const std::string &fileName, uint64_t line, const std::string &message, ...)
{
    std::lock_guard<std::mutex> lock(logMutex);

    // Setup color of output
    std::string typeColor;
    std::string typeText;
    switch (type)
    {
    case MessageType::MESSAGE_TYPE_INFO:
        typeColor = DEFAULT_COLOR_CODE;
        typeText  = "Info";
        break;
    }
    typeText = std::string("[ ") + typeText + " ]";

    // Setup %s, %i etc codes
    va_list args;
    va_start(args, message);
    char printBuffer[4096];
    vsnprintf(printBuffer, sizeof(printBuffer), message.c_str(), args);
    va_end(args);
    std::string outputMessage = printBuffer;


    // Get current date and time
    std::string currentTime;
    auto timeValue = std::time(nullptr);                    // System time value
    currentTime = std::asctime(std::localtime(&timeValue)); // Transform to date-time
    currentTime.erase(currentTime.length() - 1);            // Erase garbage


    // Save log data to file
    std::fstream logfile(LOGFILE_PATH, std::ios_base::out | std::ios_base::app); // Open existing or create new file
    if (logfile.is_open())
    {
        logfile << typeText + " : " + outputMessage << std::endl;
        logfile.close();
    }

    // Setup color and message type
    outputMessage = typeColor + typeText + DEFAULT_COLOR_CODE + " : " + outputMessage + '\n';

    // Print message
    if ((type == Logging::MessageType::MESSAGE_TYPE_ERROR) || (type == Logging::MessageType::MESSAGE_TYPE_WARNING))
    {
        std::cerr << outputMessage;
    }
    else
    {
        std::cout << outputMessage;
    }
}
