#pragma once
#include <string>
namespace mygo {
struct Loc {
  int line;
  int coloum;

  ~Loc() = default;
  Loc(const Loc&) = default;
  Loc() : line(1), coloum(1) {};

  bool operator==(const Loc& other) {
    return line == other.line && coloum == other.coloum;
  }

  Loc(int l, int c) : line(l), coloum(c) {}
  std::string ToString() {
    return std::string() + "(" + std::to_string(line) + "," +
           std::to_string(coloum) + ")";
  }
};

// duration in us
using Duration = uint64_t;
constexpr Duration NanoSec = 1;
constexpr Duration MicroSec = 1000 * NanoSec;
constexpr Duration MiliSec = 1000 * MicroSec;
constexpr Duration Sec = 1000 * MiliSec;

using IP = std::string;
using Port = int;

}  // namespace mygo
