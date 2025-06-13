#pragma once
#include <optional>
#include <vector>

#include "token.h"
namespace mygo {
class TokenStream {
 public:
  TokenStream(std::vector<uint8_t>&& src);
  void Next();
  std::optional<token::Value> Peek();
  bool Finish();

 private:
  std::optional<token::Value> cache_;
  int cache_key_ = -1;
  std::vector<uint8_t> src_;
  int pos_ = 0;
  int line_;
  int colum_;
};

}  // namespace mygo
