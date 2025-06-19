#include "token_stream.h"

#include <optional>
#include <vector>

#include "logging.h"
#include "token.h"

namespace mygo {

namespace {}  // namespace

TokenStream::TokenStream(std::vector<uint8_t>&& src) : src_(src) {}

std::optional<token::Value> TokenStream::Peek() {
  if (cache_key_ == state_.pos) return cache_;
  token::CharPtr cur_char = src_.begin() + state_.pos;
  std::optional<token::Value> v = token::Value::FromChars(cur_char, src_.end());

  cache_key_ = state_.pos;
  cache_ = v;

  if (!v) return v;

  cache_->start = Loc(state_.line, state_.colum);

  if (cache_->type() == token::Type::Enl) {
    state_.line++;
    state_.colum = 1;
  } else {
    state_.colum += cache_->len();
  }

  cache_->end = Loc(state_.line, state_.colum);

  LOG(INFO) << cache_.value() << cache_.value().start.ToString()
            << cache_.value().end.ToString();

  return v;
}

void TokenStream::Next() {
  while (true) {
    state_.pos += Peek()->len();

    if (!Peek()) return;

    if (Peek()->type() == token::Type::Space) continue;

    break;
  }
  return;
}

}  // namespace mygo
