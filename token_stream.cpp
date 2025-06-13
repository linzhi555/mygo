#include "token_stream.h"

#include <optional>
#include <vector>

#include "logging.h"
namespace mygo {

TokenStream::TokenStream(std::vector<uint8_t>&& src)
    : src_(src), pos_(0), line_(0), colum_(0) {}

std::optional<token::Value> TokenStream::Peek() {
  if (cache_key_ == pos_) return cache_;
  token::CharPtr cur_char = src_.begin() + pos_;
  std::optional<token::Value> v = token::Value::FromChars(cur_char, src_.end());

  cache_key_ = pos_;
  cache_ = v;

  return v;
}

void TokenStream::Next() {
  while (true) {
    pos_ += Peek()->len();
    if (!Peek()) return;
    if (Peek()->type() == token::Type::Space) continue;
    auto temp = Peek();
    if (temp) {
      LOG(INFO) << "next token" << temp.value() << std::endl;
    }
    return;
  }
  return;
}

}  // namespace mygo
