#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

string ToMb(string data) {
  float mb = std::stoi(data) / 1000.0;
  string mb_string = to_string(static_cast<int>(mb));
  return mb_string;
}

Process::Process(int pid) : pid_(pid) {
  uid_user_map_ = LinuxParser::GetUIDUserMap();
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  return (LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) /
         static_cast<float>(UpTime());
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return ToMb(LinuxParser::Ram(pid_)); }

string Process::User() {
  string user = LinuxParser::User(pid_);
  return uid_user_map_.find(user)->second;
}

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process& a) {
  return a.CpuUtilization() < CpuUtilization();
}
