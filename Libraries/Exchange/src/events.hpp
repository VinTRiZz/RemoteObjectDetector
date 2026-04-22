#pragma once

#include <string>
#include <map>

namespace Protocol
{

// TODO: RENAME

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
    DetectorSoftVersionAdded,
    DetectorSoftVersionChanged,
    DetectorSoftVersionRemoved,

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
 * @brief Headers for events
 */
namespace EventHeaders
{
const std::string HEADER_DEVICE {"device"};
}


/**
 * @brief The Event class   Event of the server, detector, etc.
 */
class Event
{
public:
    Event() = default;
    explicit Event(const std::string& initialTxt);
    virtual ~Event() = default;

    bool readRaw(const std::string& txt) noexcept;
    std::string toRaw() const;

    void setHeader(const std::string& header, const std::string& value);
    std::string getHeader(const std::string& headerName) const;

    void setType(EventType etype);
    EventType   getType() const noexcept;

    void setPayload(const std::string& iData);
    void setPayload(std::string&& iData);
    std::string getPayload() const;

private:
    std::map<std::string, std::string>  m_headers;
    EventType                           m_type {EventType::Undefined};
    std::string                         m_payload;
};

}
