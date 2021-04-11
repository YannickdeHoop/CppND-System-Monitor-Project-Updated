#ifndef PROCESS_H
#define PROCESS_H

#include <map>
#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process& a);

 private:
  int pid_ = -1;
  std::map<std::string, std::string> uid_user_map_;
};

#endif
