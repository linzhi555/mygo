#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace mygo {
namespace token {

#define TokenList                                                              \
  T(Dot, "."), T(Var, "var"), T(Const, "const"), T(Struct, "struct"),          \
      T(For, "for"), T(Typedef, "type"), T(Equal, "=="), T(Assign, "="),       \
      T(Plus, "+"), T(Sub, "-"), T(Star, "*"), T(Slash, "/"), T(Func, "func"), \
      T(LBrace, "{"), T(RBrace, "}"), T(LParent, "("), T(RParent, ")"),        \
      T(Comma, ","), T(Colon, ":"),

enum class Type {
#define T(a, b) a
  TokenList
#undef T
};

// enum Type {
//     Dot,
//     Var,
//     Const,
//     Struct,
//     Interface,
//     For,
//     Typedef,
//     Space,
//     Int,
//     Symbol,
//     Float,
//     Str,
//     Func,
//     Assign,
//     Equal,
//     NotEqual,
//     Plus,
//     Sub,
//     Star,
//     Slash,
//     RSlash,
//     LBrace,
//     RBrace,
//     LParent,
//     RParent,
//     Comma,
//     Colon,
//     EndL,
//     EndF,
// };

using CharPtr = std::vector<uint8_t>::iterator;

class Value {
 public:
  Value(Type t, int len);
  static std::optional<Value> FromChars(CharPtr cur, CharPtr end);
  Type type() { return type_; }
  std::string str() { return str_data_; }
  int len() { return len_; };

  double f() { return f_data_; }
  int i() { return i_data_; }

 private:
  Type type_;
  int len_;
  std::string str_data_;
  double f_data_;
  int i_data_;
};

}  // namespace token
}  // namespace mygo
