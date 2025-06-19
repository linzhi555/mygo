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

  struct State {
    int pos = 0;
    int line = 1;
    int colum = 1;
  };

  class StateGuard {
    TokenStream& stream_;
    State state_;
    bool canceled_ = false;

   public:
    StateGuard(TokenStream& stream) : stream_(stream), state_(stream.Save()) {}
    void Cancel() { canceled_ = true; };

    ~StateGuard() {
      if (!canceled_) {
        stream_.Load(state_);
      }
    }
  };

  State Save() { return state_; };
  void Load(State s) { state_ = s; };

 private:
  std::optional<token::Value> cache_;
  int cache_key_ = -1;
  std::vector<uint8_t> src_;
  State state_;
};

}  // namespace mygo
