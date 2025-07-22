#include "value.h"

#include "token.h"

namespace mygo {

std::string Value::ToString() {
  if (type == INT) return std::to_string(std::get<int>(data));

  if (type == STR) return std::get<std::string>(data);

  if (type == BOOL)
    return std::get<bool>(data) ? std::string("true") : std::string("false");

  if (type == FLOAT) return std::to_string(std::get<float>(data));

  if (type == FUNC) return std::string("func(") + As<Func>().ToString() + ")";

  if (type == STRUCT) return std::string("Object");

  return std::string();
}

std::optional<Value> Value::Operator(token::Type tk) {
  auto tt = this->type;
  if (tk == token::Type::Sub) {
    if (tt == INT) {
      return Make<int>(-1 * this->As<int>());
    }

    if (tt == FLOAT) {
      return Make<float>(-1 * this->As<float>());
    }
  }

  return std::nullopt;
}

#define DO_BINARAY_OP(TK_TYPE, OP)                                \
  if (tk == TK_TYPE) {                                            \
    if (tt == INT && ot == INT) {                                 \
      return Make<int>(this->As<int>() OP other.As<int>());       \
    }                                                             \
                                                                  \
    if (tt == INT && ot == FLOAT) {                               \
      return Make<float>(this->As<int>() OP other.As<float>());   \
    }                                                             \
                                                                  \
    if (tt == FLOAT && ot == INT) {                               \
      return Make<float>(this->As<float>() OP other.As<int>());   \
    }                                                             \
                                                                  \
    if (tt == FLOAT && ot == FLOAT) {                             \
      return Make<float>(this->As<float>() OP other.As<float>()); \
    }                                                             \
  }

std::optional<Value> Value::Operator(token::Type tk, Value& other) {
  auto ot = other.type;
  auto tt = this->type;

  DO_BINARAY_OP(token::Type::Plus, +);
  DO_BINARAY_OP(token::Type::Sub, -);
  DO_BINARAY_OP(token::Type::Star, *);
  DO_BINARAY_OP(token::Type::Slash, /);

  return std::nullopt;
}

}  // namespace mygo
