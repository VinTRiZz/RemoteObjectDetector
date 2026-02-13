#include "statusfunctions.hpp"

#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>
#include <vector>
#include <thread>
#include <algorithm>

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }

    pclose(pipe);
    return result;
}

std::vector<size_t> get_cpu_times() {
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix.
    std::vector<size_t> times;
    for (size_t time; proc_stat >> time; times.push_back(time));
    return times;
}

bool get_cpu_times(size_t &idle_time, size_t &total_time) {
    const std::vector<size_t> cpu_times = get_cpu_times();
    if (cpu_times.size() < 4)
        return false;
    idle_time = cpu_times[3];
    total_time = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);
    return true;
}

float cpuLoad()
{
    size_t previous_idle_time=0, previous_total_time=0;
    size_t idle_time{}, total_time{};

    float utilization = 0;
    for (int i = 0; i < 2; i++)
    {
        get_cpu_times(idle_time, total_time);
        const float idle_time_delta = idle_time - previous_idle_time;
        const float total_time_delta = total_time - previous_total_time;
        utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
        previous_idle_time = idle_time;
        previous_total_time = total_time;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return utilization;
}

float cpuTemperature()
{
    auto tempers = exec("sensors | grep Tctl");
    auto currentPos = std::find(tempers.begin(), tempers.end(), '+') + 1;
    auto endPos = std::find(currentPos, tempers.end(), 'C') - 2;
    return std::stof(std::string(currentPos, endPos));
}

std::string getStartTime()
{
    return exec("who -b | awk '{print $3}' | date +\"%d.%m.%Y\" -f -");
}
