#include "token_stream.h"

#include <optional>
#include <vector>
namespace mygo {

TokenStream::TokenStream(std::vector<uint8_t>&& src)
    : src_(src), pos_(0), line_(0), colum_(0) {}

std::optional<token::Value> TokenStream::Peek() {
  if (cache_) return cache_;
  token::CharPtr cur_char = src_.begin() + pos_;
  std::optional<token::Value> v = token::Value::FromChars(cur_char, src_.end());
  if (!v) return std::nullopt;
  cache_ = v;
  return v;
}

std::optional<token::Value> TokenStream::Next() {
  if (!Peek()) return std::nullopt;
  pos_ += Peek()->len();
  auto temp = Peek();
  cache_ = std::nullopt;
  return temp;
}

}  // namespace mygo
