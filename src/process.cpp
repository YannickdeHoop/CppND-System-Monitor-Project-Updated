#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

std::string ToMb(std::string data){
    float mb = std::stoi(data) / 1000.0;
    std::string mb_string = std::to_string(mb);
    return mb_string;
}

Process::Process(int pid) : pid_(pid)
{
  uid_user_map_ = LinuxParser::GetUIDUserMap();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return ToMb(LinuxParser::Ram(pid_)); }

// TODO: Return the user (name) that generated this process
string Process::User() {
    std::string user = LinuxParser::User(pid_);
    return uid_user_map_.find(user)->second; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }
