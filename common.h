#include <string>
namespace mygo {
struct Loc {
  int line;
  int coloum;

  ~Loc() = default;
  Loc(const Loc&) = default;
  Loc() : line(1), coloum(1) {};

  Loc(int l, int c) : line(l), coloum(c) {}
  std::string ToString() {
    return std::string() + "(" + std::to_string(line) + "," +
           std::to_string(coloum) + ")";
  }
};

inline std::ostream& operator<<(std::ostream& o, Loc loc) {
  o << loc.ToString();
  return o;
}

}  // namespace mygo
