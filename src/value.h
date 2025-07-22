#pragma once

#include <cassert>
#include <functional>
#include <string>
#include <variant>
#include <vector>

#include "ast.h"

namespace mygo {

class VM;
class Func {
 public:
  using Userdef = ast::Function*;
  using Builtin = std::function<void(VM*, std::vector<ast::Expr*>&)>;

  bool isBuiltin() { return is_builtin_; }

  Func(Userdef uf) {
    func_ = uf;
    is_builtin_ = false;
  };

  Func(Builtin bf) {
    func_ = bf;
    is_builtin_ = true;
  };

  Userdef AsUserDef() {
    assert(!is_builtin_);
    return std::get<Userdef>(func_);
  }

  Builtin AsBuiltin() {
    assert(is_builtin_);
    return std::get<Builtin>(func_);
  }

  std::string ToString() {
    return is_builtin_ ? "buildin" : AsUserDef()->func_name;
  };

  ~Func() = default;

  bool is_builtin_;
  std::variant<Userdef, Builtin> func_;
};

// template <typename T, typename Variant>
// concept OfVariant = std::constructible_from<Variant, T>;

class Value {
 public:
  using Type_t = std::string_view;
  static constexpr Type_t NIL = "nil";
  static constexpr Type_t BOOL = "bool";
  static constexpr Type_t STR = "bool";
  static constexpr Type_t INT = "int";
  static constexpr Type_t FLOAT = "float";
  static constexpr Type_t STRUCT = "struct";
  static constexpr Type_t FUNC = "func";

  Type_t type = NIL;
  bool is_builtin = true;

  using Data = std::variant<bool, std::string, int, float, Func>;
  Data data;

  template <typename T>
  static Value Make(T v) {
    // static_assert(OfVariant<T, Data>);
    Value value;

    value.is_builtin = true;
    if constexpr (std::is_same_v<T, int>) {
      value.type = INT;
    } else if constexpr (std::is_same_v<T, bool>) {
      value.type = BOOL;
    } else if constexpr (std::is_same_v<T, std::string>) {
      value.type = STR;
    } else if constexpr (std::is_same_v<T, float>) {
      value.type = FLOAT;
    } else if constexpr (std::is_same_v<T, Func>) {
      value.type = FUNC;
    }

    value.data = std::move(v);
    return value;
  }

  template <typename T>
  T As() {
    // static_assert(OfVariant<T, Data>);
    return std::get<T>(data);
  }

  std::string ToString();

  std::optional<Value> Operator(token::Type t);
  std::optional<Value> Operator(token::Type t, Value& other);
};

}  // namespace mygo
