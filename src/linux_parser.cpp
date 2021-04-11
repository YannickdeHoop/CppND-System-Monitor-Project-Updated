#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::ifstream;
using std::istringstream;
using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
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

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

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

float LinuxParser::MemoryUtilization() {
  string line, key, value, unit;
  float total_memory = 0, slab = 0, available_memory = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          total_memory = stof(value);
        }
        if (key == "MemAvailable:") {
          available_memory = stof(value);
        }
        if (key == "Slab:") {
          slab = stof(value);
        }
      }
    }
  }
  return 1 - (available_memory - slab) / (total_memory - slab);
}

long LinuxParser::UpTime() {
  string line, up_time, idle_time;
  ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return stoi(up_time);
}

long LinuxParser::Jiffies() { return IdleJiffies() + ActiveJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  vector<string> pid_stat;
  string stat_string;
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, stat_string, ' ')) {
      pid_stat.push_back(stat_string);
    }
    return stol(pid_stat.at(13)) + stol(pid_stat.at(14)) +
           stol(pid_stat.at(15)) + stol(pid_stat.at(16));
  }
  return 0;
}

long LinuxParser::ActiveJiffies() {
  string cpu_name, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      line;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> cpu_name >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest;
    return stoi(user) + stoi(nice) + stoi(system) + stoi(irq) + stoi(softirq) +
           stoi(steal) + stoi(guest);
  }
  return 0;
}

long LinuxParser::IdleJiffies() {
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, line;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal >> guest;
    return stoi(idle) + stoi(iowait);
  }
  return 0;
}

float LinuxParser::CpuUtilization() {
  return static_cast<float>(ActiveJiffies()) / static_cast<float>(Jiffies());
}

int LinuxParser::TotalProcesses() {
  {
    string line, key, value;
    int n_processes;

    ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "processes") {
            n_processes = stoi(value);
            return n_processes;
          }
        }
      }
    }
    return 0;
  }
}

int LinuxParser::RunningProcesses() {
  string line, key, value;
  int procs_running = 0;

  ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          procs_running = stoi(value);
          break;
        }
      }
    }
    return procs_running;
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string cmd, line;
  ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    istringstream linestream(line);
    linestream >> cmd;
    return cmd;
  }
  return std::string();
}

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
  return string();
}

string LinuxParser::User(int pid) {
  string line, key, value;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  vector<string> pid_stat;
  string stat_string;
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, stat_string, ' ')) {
      pid_stat.push_back(stat_string);
    }
    return UpTime() - (stol(pid_stat.at(21)) / sysconf(_SC_CLK_TCK));
  }
  return 0;
}

std::map<string, string> LinuxParser::GetUIDUserMap() {
  std::map<string, string> uid_user_map = {};
  string user_name, password, uid, group_id, user_id_info, home_dir, cmd, line;
  ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      istringstream linestream(line);
      while (linestream >> user_name >> password >> uid >> group_id >>
             user_id_info >> home_dir >> cmd) {
        uid_user_map.insert({uid, user_name});
      }
    }
    return uid_user_map;
  }
  return std::map<string, string>{};
}
