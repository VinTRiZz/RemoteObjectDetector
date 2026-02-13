#ifndef STATUSFUNCTIONS_HPP
#define STATUSFUNCTIONS_HPP

#include <string>

std::string exec(const char* cmd);

float cpuLoad();

float cpuTemperature();

std::string getStartTime();

#endif // STATUSFUNCTIONS_HPP
