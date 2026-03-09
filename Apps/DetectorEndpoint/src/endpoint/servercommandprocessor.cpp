#include "servercommandprocessor.hpp"

#include <Components/Logger/Logger.h>

ServerCommandProcessor::ServerCommandProcessor() :
    EventProcessor()
{
    setProcessorName("Server command processor");
}

ServerCommandProcessor::~ServerCommandProcessor()
{

}
