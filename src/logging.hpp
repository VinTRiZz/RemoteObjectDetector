#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>

namespace Logging
{

enum MessageType
{
    MESSAGE_TYPE_EMPTY, // To print just text, without data, place etc. labels

    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,

    // For operations like getting photo and processing
    MESSAGE_TYPE_OPRESULT_SUCCESS,
    MESSAGE_TYPE_OPRESULT_ERROR
};

void log(MessageType type, const std::string& fileName, uint64_t line, const std::string& message, ...);

const std::string LOGFILE_PATH {"./ObjectDetector.log"};

#define LOG_EMPTY(message, ...)     log(Logging::MessageType::MESSAGE_TYPE_EMPTY,   __FILE__, __LINE__,  message, ##__VA_ARGS__)

#define LOG_INFO(message, ...)      log(Logging::MessageType::MESSAGE_TYPE_INFO,    __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_WARNING(message, ...)   log(Logging::MessageType::MESSAGE_TYPE_WARNING, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...)     log(Logging::MessageType::MESSAGE_TYPE_ERROR,   __FILE__, __LINE__, message, ##__VA_ARGS__)

#define LOG_OPRES_SUCCESS(message, ...)   log(Logging::MessageType::MESSAGE_TYPE_OPRESULT_SUCCESS, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define LOG_OPRES_ERROR(message, ...)     log(Logging::MessageType::MESSAGE_TYPE_OPRESULT_ERROR,   __FILE__, __LINE__,   message, ##__VA_ARGS__)
}

#endif // LOGGING_HPP
