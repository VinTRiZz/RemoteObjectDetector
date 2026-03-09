#include "abstractendpoint.hpp"

void AbstractEndpoint::setEventProcessor(const Protocol::EventProcessorPtr &pEventProcessor)
{
    m_pEventProcessor = pEventProcessor;
}
