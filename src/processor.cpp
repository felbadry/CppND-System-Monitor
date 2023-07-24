#include "processor.h"
#include "linux_parser.h"
#include <numeric>
#include <vector>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    auto strtimes = LinuxParser::CpuUtilization();
    std::vector<long> times(strtimes.size());
    std::transform(strtimes.begin(), strtimes.end(), std::back_inserter(times), 
                    [](const std::string& str) { return std::stol(str); });
    long total = std::accumulate(times.begin(), times.end(), 0);
    long idletime = times.at(3);
    return (total - idletime) / total;
 }