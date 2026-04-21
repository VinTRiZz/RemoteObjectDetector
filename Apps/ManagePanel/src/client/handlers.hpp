#pragma once

#include "handlerbase.hpp"

namespace Web {

class Server;
class Detector;

/**
 * @brief The ServerHandler class Handler of a server, that holds pointer to it
 */
class ServerHandler : public HandlerBase<Server>
{
public:
    using HandlerBase<Server>::HandlerBase;

private:
    friend class ServerRegistry; // For server handler updating
};


/**
 * @brief The DetectorHandler class Handler of a detector object, that holds pointer to it
 */
class DetectorHandler : public HandlerBase<Detector>
{
public:
    using HandlerBase<Detector>::HandlerBase;

private:
    friend class DetectorServer; // For server handler updating
};

} // namespace Web
