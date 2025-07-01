#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common.h"
namespace mygo {

namespace token {

#define TokenList     \
  T(Dot, ".")         \
  T(True, "true")     \
  T(False, "flase")   \
  T(Var, "var")       \
  T(Const, "const")   \
  T(Struct, "struct") \
  T(For, "for")       \
  T(If, "if")         \
  T(Typedef, "type")  \
  T(Equal, "==")      \
  T(Less, "<")        \
  T(Greater, ">")     \
  T(Assign, "=")      \
  T(Plus, "+")        \
  T(Sub, "-")         \
  T(Star, "*")        \
  T(Slash, "/")       \
  T(Func, "func")     \
  T(Return, "return") \
  T(LBrace, "{")      \
  T(RBrace, "}")      \
  T(LParent, "(")     \
  T(RParent, ")")     \
  T(Comma, ",")       \
  T(Colon, ":")       \
  T(Semicolon, ";")   \
  K(Space, "SPACE")   \
  K(Int, "Int")       \
  K(Float, "Float")   \
  K(Str, "Str")       \
  K(Symbol, "Symbol") \
  K(Enl, "Enl")       \
  K(Enf, "Enf")

enum class Type {
#define T(a, b) a,
#define K(a, b) a,
  TokenList
#undef K
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

struct Value {
 public:
  Value() = default;
  Value(Type t, int len) : type_(t), len_(len) {};
  static Value CreateInt(int len, int data) {
    Value v(Type::Int, len);
    v.i_data_ = data;
    return v;
  }

  static Value CreateFloat(int len, float data) {
    Value v(Type::Float, len);
    v.f_data_ = data;
    return v;
  }

  static Value CreateStr(int len, std::string data) {
    Value v(Type::Str, len);
    v.str_data_ = data;
    return v;
  }

  static Value CreateSymbol(int len, std::string data) {
    Value v(Type::Symbol, len);
    v.str_data_ = data;
    return v;
  }

  static std::optional<Value> FromChars(CharPtr cur, CharPtr end);
  Type type() { return type_; }
  std::string str() { return str_data_; }
  int len() { return len_; };

  double f() { return f_data_; }
  int i() { return i_data_; }

  std::string debug();

  Loc start = Loc(1, 1);
  Loc end = Loc(1, 1);
  Type type_;
  int len_;
  std::string str_data_;
  double f_data_;
  int i_data_;
};

std::ostream& operator<<(std::ostream& stream, Value v);
std::ostream& operator<<(std::ostream& stream, Type t);

std::string debug_type(token::Type t);
}  // namespace token
}  // namespace mygo
