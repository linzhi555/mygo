#pragma once
#include <string>
#include <unordered_map>
#include <variant>

#include "ast.h"
namespace mygo {
namespace vm {

class Object;

class Value {
 public:
  enum class Type { Bool, Str, Int, Float, Object };

  Type type;
  bool is_builtin = true;
  std::variant<bool, std::string, int, float, Object*> data;

  template <typename T>
  static Value Make(T v) {
    Value value;

    value.is_builtin = true;
    if constexpr (std::is_same_v<T, int>) {
      value.type = Type::Int;
    } else if constexpr (std::is_same_v<T, bool>) {
      value.type = Type::Bool;
    } else if constexpr (std::is_same_v<T, std::string>) {
      value.type = Type::Str;
    } else if constexpr (std::is_same_v<T, float>) {
      value.type = Type::Float;
    } else if constexpr (std::is_same_v<T, Object*>) {
      value.is_builtin = false;
      value.type = Type::Object;
    }

    value.data = std::move(v);
    return value;
  }
  std::string ToString() {
    switch (type) {
      case Type::Int:
        return std::to_string(std::get<int>(data));
      case Type::Str:
        return std::get<std::string>(data);

      case Type::Bool:
        return std::get<bool>(data) ? std::string("true")
                                    : std::string("false");
      case Type::Float:
        return std::to_string(std::get<float>(data));

      case Type::Object:
        return std::string("Object");
    }

    return std::string();
  }
};

class VM {
 public:
  void run(ast::NodePtr<ast::Root>& root);
  std::unordered_map<std::string, Value> globals_;
};

}  // namespace vm
}  // namespace mygo
