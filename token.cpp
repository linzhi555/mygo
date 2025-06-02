#include "token.h"

#include <tuple>
#include <vector>

namespace mygo {
namespace token {

std::vector<std::tuple<std::string, Type>> simple_tokens{
#define T(a, b) {b, Type::a}
    TokenList
#undef T
};

Value::Value(Type t, int len) : type_(t), len_(len) {}

std::optional<Value> token::Value::FromChars(CharPtr cur, CharPtr end) {
  for (auto& [l, t] : simple_tokens) {
    for (size_t i = 0; i < l.length(); i++) {
      if (cur + i == end) continue;
      if (*(cur + i) != l[i]) continue;
    }
    return Value(t, l.length());
  }

  return std::nullopt;
}
}  // namespace token
}  // namespace mygo
