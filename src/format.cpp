#include "format.h"

#include <string>

constexpr int HOUR = 3600;
constexpr int MIN = 60;

std::string Format::ElapsedTime(long seconds) {
  int hour = seconds / HOUR;
  int seconds_left = seconds % HOUR;
  int minute = seconds_left / MIN;
  int second = seconds_left % MIN;

  return std::to_string(hour) + ":" + std::to_string(minute) + ":" +
         std::to_string(second);
}
