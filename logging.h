#pragma once
#include <iostream>

enum Serverity {
  NOLOG = 0,
  ERROR = 1,
  WARNING = 2,
  INFO = 3,
  FATAL = 4,
};

#define LOG(serverity) log_stream(serverity)

std::ostream& log_stream(Serverity s);
void set_debug_level(Serverity s);
