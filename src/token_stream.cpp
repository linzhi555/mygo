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

  cache_->start = state_.loc;
  cache_->end = cache_->start;
  if (cache_->type() == token::Type::Enl) {
    cache_->end.line++;
    cache_->end.coloum = 1;
  } else {
    cache_->end.coloum += cache_->len();
  }

  if (!v) return v;

  LOG(INFO) << cache_.value() << cache_.value().start.ToString()
            << cache_.value().end.ToString();

  return v;
}

void TokenStream::Next() {
  if (Peek()) {
    state_.last_tk = Peek().value();
  }
  while (true) {
    state_.loc = Peek()->end;
    state_.pos += Peek()->len();
    Peek();

    if (!Peek()) return;

    if (Peek()->type() == token::Type::Space) continue;

    break;
  }
  return;
}

}  // namespace mygo
