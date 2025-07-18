#include "value.h"

#include "token.h"

namespace mygo {

std::string Value::ToString() {
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

#define DO_BINARAY_OP(TK_TYPE, OP)                                \
  if (tk == TK_TYPE) {                                            \
    if (tt == Int && ot == Int) {                                 \
      return Make<int>(this->As<int>() OP other.As<int>());       \
    }                                                             \
                                                                  \
    if (tt == Int && ot == Float) {                               \
      return Make<float>(this->As<int>() OP other.As<float>());   \
    }                                                             \
                                                                  \
    if (tt == Float && ot == Int) {                               \
      return Make<float>(this->As<float>() OP other.As<int>());   \
    }                                                             \
                                                                  \
    if (tt == Float && ot == Float) {                             \
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
