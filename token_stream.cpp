#include "token_stream.h"

#include <optional>
#include <vector>

#include "logging.h"
#include "token.h"

namespace mygo {

namespace {}  // namespace

TokenStream::TokenStream(std::vector<uint8_t>&& src)
    : src_(src), pos_(0), line_(1), colum_(1) {}

std::optional<token::Value> TokenStream::Peek() {
  if (cache_key_ == pos_) return cache_;
  token::CharPtr cur_char = src_.begin() + pos_;
  std::optional<token::Value> v = token::Value::FromChars(cur_char, src_.end());

  cache_key_ = pos_;
  cache_ = v;

  if (!v) return v;

  cache_->start = Loc(line_, colum_);

  if (cache_->type() == token::Type::Enl) {
    line_++;
    colum_ = 1;
  } else {
    colum_ += cache_->len();
  }

  cache_->end = Loc(line_, colum_);

  LOG(INFO) << cache_.value() << cache_.value().start.ToString()
            << cache_.value().end.ToString();

  return v;
}

void TokenStream::Next() {
  while (true) {
    pos_ += Peek()->len();

    if (!Peek()) return;

    if (Peek()->type() == token::Type::Space) continue;

    break;
  }
  return;
}

}  // namespace mygo
