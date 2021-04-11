#include "format.h"

#include <iomanip>
#include <string>

constexpr int HOUR = 3600;
constexpr int MIN = 60.0;

std::string Format::ElapsedTime(long seconds) {
  int hour = seconds / HOUR;
  int seconds_left = seconds % HOUR;
  int minute = seconds_left / MIN;
  int second = seconds_left % MIN;

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << hour << ":";
  ss << std::setfill('0') << std::setw(2) << minute << ":";
  ss << std::setfill('0') << std::setw(2) << second;

  return ss.str();
}
