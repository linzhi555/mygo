#include "token.h"

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace mygo {
namespace token {

std::vector<std::tuple<std::string, Type>> simple_tokens{
#define T(a, b) {b, Type::a},
#define K(a, b)
    TokenList
#undef K
#undef T
};

std::unordered_map<Type, std::string> token_debug_map{
#define T(a, b) {Type::a, b},
#define K T
    TokenList
#undef K
#undef T
};

std::ostream& operator<<(std::ostream& stream, token::Type v) {
  stream << static_cast<int>(v);
  return stream;
}

std::string debug_type(token::Type t) {
  auto it = token_debug_map.find(t);
  if (it != token_debug_map.end()) {
    return token_debug_map[t];
  } else {
    return "unrecognizeType";
  }
}

std::ostream& operator<<(std::ostream& stream, token::Value v) {
  stream << "[" << debug_type(v.type());

  if (v.type() == Type::Int) {
    stream << ":" << std::to_string(v.i());
  }

  if (v.type() == Type::Float) {
    stream << ":" << std::to_string(v.f());
  }

  if (v.type() == Type::Str) {
    stream << ":" << v.str();
  }

  if (v.type() == Type::Symbol) {
    stream << ":" << v.str();
  }

  stream << "]";
  return stream;
}

std::string Value::debug() {
  std::ostringstream sstream;
  sstream << *this;
  return sstream.str();
}

std::optional<Value> int_from_chars(CharPtr start, CharPtr end) {
  int len = 0;
  int res = 0;
  for (CharPtr temp = start; temp != end; temp++) {
    if (*temp <= '9' && *temp >= '0') {
      res = res * 10 + (*temp - '0');
      len++;
    } else {
      break;
    }
  }

  if (len > 0) {
    return Value::CreateInt(len, res);
  }

  return std::nullopt;
}
namespace {

float decimal_weight_after_zero(unsigned int offset) {
  float weight = 1.0;
  for (unsigned int i = 0; i < offset; i++) {
    weight /= 10.0;
  }
  return weight;
}

bool is_symbol_head_ele(uint8_t c) {
  if (c >= 'a' && c <= 'z') return true;
  if (c >= 'A' && c <= 'Z') return true;
  if (c == '_') return true;
  return false;
}

bool is_digit(uint8_t c) {
  if (c >= '0' && c <= '9') return true;
  return false;
}

bool is_symbol_ele(uint8_t c) {
  if (is_symbol_head_ele(c)) return true;
  if (is_digit(c)) return true;
  return false;
}

}  // namespace

std::optional<Value> symbol_from_chars(CharPtr start, CharPtr end) {
  if (!is_symbol_head_ele(*start)) return std::nullopt;

  auto cur = start + 1;
  for (; cur != end; cur++) {
    if (!is_symbol_ele(*cur)) break;
  }

  if (cur == end) return std::nullopt;

  std::string s(start, cur);

  return Value::CreateSymbol(s.length(), s);
}

std::optional<Value> str_from_chars(CharPtr start, CharPtr end) {
  if (*start != '"') return std::nullopt;
  auto cur = start + 1;
  for (; cur != end; cur++) {
    if (*cur == '"') break;
  }

  if (cur == end) return std::nullopt;

  std::string s(start + 1, cur);

  return Value::CreateStr(s.length() + 2, s);
}

std::optional<Value> float_from_chars(CharPtr start, CharPtr end) {
  auto n1 = int_from_chars(start, end);
  if (!n1) return std::nullopt;
  auto temp = start + n1->len();
  if (temp == end) return std::nullopt;
  if (*temp != '.') return std::nullopt;
  auto n2 = int_from_chars(temp + 1, end);
  if (!n2) return std::nullopt;
  return Value::CreateFloat(
      n1->len() + n2->len() + 1,
      n1->i() + decimal_weight_after_zero(n2.value().len()) * n2->i());
}

std::optional<Value> endl_from_chars(CharPtr start, CharPtr end) {
  if ((start + 1) != end && (*start == '\r' && *(start + 1) == '\n')) {
    return Value(token::Type::Enl, 2);
  }

  if (*start == '\r' || *start == '\n') {
    return Value(token::Type::Enl, 1);
  }

  if (*start == ';') {
    return Value(token::Type::Enl, 1);
  }

  return std::nullopt;
}

std::optional<Value> token::Value::FromChars(CharPtr start, CharPtr end) {
  if (start == end) {
    return token::Value(Type::Enf, 0);
  }

  if (auto v = endl_from_chars(start, end)) {
    return v;
  }

  if (*start == ' ' || *start == '\t') {
    int i = 1;
    for (; start + i != end; i++) {
      if (*(start + i) != ' ' && *(start + i) != '\t') {
        break;
      }
    }
    return token::Value(Type::Space, i);
  }

  for (auto& [l, t] : simple_tokens) {
    bool ismatch = true;
    for (size_t i = 0; i < l.length(); i++) {
      if (start + i == end || (*(start + i) != l[i])) {
        ismatch = false;
        break;
      }
    }
    if (ismatch) {
      return Value(t, l.length());
    }
  }

  if (auto v = float_from_chars(start, end)) {
    return v;
  }

  if (auto v = int_from_chars(start, end)) {
    return v;
  }

  if (auto v = str_from_chars(start, end)) {
    return v;
  }

  if (auto v = symbol_from_chars(start, end)) {
    return v;
  }

  return std::nullopt;
}
}  // namespace token
}  // namespace mygo
