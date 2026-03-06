#pragma once

#include <string>

namespace Protocol
{

/**
 * @brief The EventType enum    Type of an event on a server or device
 */
enum EventType : short
{
    Undefined = -1,

    // Server events
    ServerStarted,
    ServerStopped,
    ServerShutdownCalled,
    ServerRebootCalled,
    ServerAlert, // Critical temperature, available space, etc.

    // Detector software
    VersionAdded,
    VersionChanged,
    VersionRemoved,

    // Regular
    DetectorConnected,
    DetectorDisconnected,
    DetectorAlert, // Critical temperature, available space, etc.

    // Detection
    DetectedObject,
    FailedObjectDetection,
};


/**
 * @brief toString  Conversion EventType into string for display, etc.
 * @param etype     Event enum value
 * @return          Not NULL string with name of event
 * @throws std::invalid_argument on etype not in @enum EventType
 */
std::string toString(EventType etype);


/**
 * @brief The Event class   Event of the server, detector, etc.
 */
struct Event
{
    std::string device;
    EventType   type {EventType::Undefined};
    std::string payload;

    bool readRaw(const std::string& txt) noexcept;
    static Event fromRaw(const std::string& txt);
    std::string toRaw() const;
};

}
