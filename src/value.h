#pragma once

#include <string>
#include <variant>

#include "ast.h"

namespace mygo {

class Value {
 public:
  using Type_t = std::string_view;
  static constexpr Type_t Nil = "nil";
  static constexpr Type_t Bool = "bool";
  static constexpr Type_t Str = "bool";
  static constexpr Type_t Int = "int";
  static constexpr Type_t Float = "float";
  static constexpr Type_t Struct = "struct";
  static constexpr Type_t Func = "func";

  Type_t type = Nil;
  bool is_builtin = true;

  std::variant<bool, std::string, int, float, ast::Function*> data;

  template <typename T>
  static Value Make(T v) {
    Value value;

    value.is_builtin = true;
    if constexpr (std::is_same_v<T, int>) {
      value.type = Int;
    } else if constexpr (std::is_same_v<T, bool>) {
      value.type = Bool;
    } else if constexpr (std::is_same_v<T, std::string>) {
      value.type = Str;
    } else if constexpr (std::is_same_v<T, float>) {
      value.type = Float;
    } else if constexpr (std::is_same_v<T, ast::Function*>) {
      value.type = Func;
    }

    value.data = std::move(v);
    return value;
  }

  template <typename T>
  T As() {
    return std::get<T>(data);
  }

  std::string ToString() {
    if (type == Int) return std::to_string(std::get<int>(data));

    if (type == Str) return std::get<std::string>(data);

    if (type == Bool)
      return std::get<bool>(data) ? std::string("true") : std::string("false");

    if (type == Float) return std::to_string(std::get<float>(data));

    if (type == Func)
      return std::string("func(") + As<ast::Function*>()->func_name + ")";

    if (type == Struct) return std::string("Object");

    return std::string();
  }
};

}  // namespace mygo
