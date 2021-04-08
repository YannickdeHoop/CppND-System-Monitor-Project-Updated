#include "linux_parser.h"
#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    return LinuxParser::CpuUtilization(); }
