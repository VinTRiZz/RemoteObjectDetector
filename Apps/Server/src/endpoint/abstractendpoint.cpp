#include "abstractendpoint.hpp"

AbstractEndpoint::AbstractEndpoint(Protocol::EventProcessor &eventProcessor) :
    m_eventProcessor {eventProcessor}
{

}
