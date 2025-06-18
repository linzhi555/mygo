#include <string>
namespace mygo {
struct Loc {
  int line;
  int coloum;

  Loc(int l, int c) : line(l), coloum(c) {}
  std::string ToString() {
    return std::string() + "(" + std::to_string(line) + "," +
           std::to_string(coloum) + ")";
  }
};
}  // namespace mygo
