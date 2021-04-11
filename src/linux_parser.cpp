#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::string line, key, value, unit;
  float total_memory = 0, slab = 0, available_memory = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          total_memory = std::stof(value);
        }
        if (key == "MemAvailable:") {
          available_memory = std::stof(value);
        }
        if (key == "Slab:") {
          slab = std::stof(value);
        }
      }
    }
  }
  return 1 - (available_memory - slab) / (total_memory - slab);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::string line, up_time, idle_time;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return std::stoi(up_time);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return IdleJiffies() + ActiveJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::vector<std::string> pid_stat;
  std::string stat_string;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, stat_string, ' ')) {
      pid_stat.push_back(stat_string);
    }
    return std::stol(pid_stat.at(13)) + std::stol(pid_stat.at(14)) +
           std::stol(pid_stat.at(15)) + std::stol(pid_stat.at(16));
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::string cpu_name, user, nice, system, idle, iowait, irq, softirq, steal,
      guest, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu_name >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest;
    return std::stoi(user) + std::stoi(nice) + std::stoi(system) +
           std::stoi(irq) + std::stoi(softirq) + std::stoi(steal) +
           std::stoi(guest);
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::string user, nice, system, idle, iowait, irq, softirq, steal, guest,
      line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal >> guest;
    return std::stoi(idle) + std::stoi(iowait);
  }
  return 0;
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  return static_cast<float>(ActiveJiffies()) / static_cast<float>(Jiffies());
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  {
    std::string line, key, value;
    int n_processes;

    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "processes") {
            n_processes = std::stoi(value);
            return n_processes;
          }
        }
      }
    }
    return 0;
  }
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line, key, value;
  int procs_running = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          procs_running = std::stoi(value);
          break;
        }
      }
    }
    return procs_running;
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Command(int pid) {
  std::string cmd;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
    return cmd;
  }
  return std::string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string line, key, value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmRSS:") {
          return value;
        }
      }
    }
  }
}

//// TODO: Read and return the user ID associated with a process
//// REMOVE: [[maybe_unused]] once you define the function
// string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string line, key, value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return std::string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::vector<std::string> pid_stat;
  std::string stat_string;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, stat_string, ' ')) {
      pid_stat.push_back(stat_string);
    }
    return UpTime() - (std::stol(pid_stat.at(21)) / sysconf(_SC_CLK_TCK));
  }
  return 0;
}

std::map<std::string, std::string> LinuxParser::GetUIDUserMap() {
  std::map<std::string, std::string> uid_user_map = {};
  std::string user_name, password, uid, group_id, user_id_info, home_dir, cmd,
      line;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user_name >> password >> uid >> group_id >>
             user_id_info >> home_dir >> cmd) {
        uid_user_map.insert({uid, user_name});
      }
    }
    return uid_user_map;
  }
  return std::map<std::string, std::string>{};
}

// I used these, hope it helps:
// https://man7.org/linux/man-pages/man5/procfs.5.html
// for the system:

// 1) system_jiffies = active_jiffies + idle_jiffies;
// 2) active_jiffies = (user + nice + system + irq + softirq + steal + guest +
// guest_nice); in /proc/stat file 3) idle_jiffies = iowait; in /proc/stat file

// for a Process:

// 1) system_jiffies = active_jiffies + idle_jiffies;
// 2) active_jiffies = (utime + stime + cutime + cstime); in /proc/[PID]/stat
// file 3) idle_jiffies = iowait; in /proc/stat file
