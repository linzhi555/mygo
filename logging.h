#pragma once
#include <cassert>
#include <iostream>

enum Serverity : int {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  NOLOG,
};

#define LOG(serverity) log_stream(serverity)<< __FUNCTION__

#define NOT_IMPLEMENTED()           \
  do {                              \
    assert(0 && "not implemented"); \
  } while (0)

std::ostream& log_stream(Serverity s);
void set_debug_level(Serverity s);
