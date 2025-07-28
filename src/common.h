#include <chrono>
#include <string>
namespace mygo {
struct Loc {
  int line;
  int coloum;

  ~Loc() = default;
  Loc(const Loc &) = default;
  Loc() : line(1), coloum(1){};

  bool operator==(const Loc &other) {
    return line == other.line && coloum == other.coloum;
  }

  Loc(int l, int c) : line(l), coloum(c) {}
  std::string ToString() {
    return std::string() + "(" + std::to_string(line) + "," +
           std::to_string(coloum) + ")";
  }
};

using Duration = std::chrono::milliseconds;
using Time = std::chrono::steady_clock::time_point;

using IP = std::string;
using Port = int;

} // namespace mygo
