#pragma once
#include <optional>
#include <vector>

#include "token.h"
namespace mygo {
class TokenStream {
 public:
  std::string parse_error;

  TokenStream(std::vector<uint8_t>&& src);
  void Next();
  std::optional<token::Value> Peek();
  bool Finish();

  struct State {
    int pos = 0;
    Loc loc;
  };

  // A respectful  guard who guard the state the of tokenstream.
  class StateGuard {
    TokenStream& stream_;
    State state_;
    bool cancel_reload_ = false;

   public:
    StateGuard(TokenStream& stream) : stream_(stream), state_(stream.state()) {}

    // tell the guard you job is finished and you can go!
    // the last thing is tell the loc changed when you is guarding.
    void finish(Loc& state, Loc& end) {
      cancel_reload_ = true;
      state = state_.loc;
      end = stream_.state().loc;
    };

    State state() { return state_; }

    ~StateGuard() {
      if (!cancel_reload_) {
        stream_.Load(state_);
      }
    }
  };

  State state() { return state_; };
  void Load(State s) { state_ = s; };

 private:
  std::optional<token::Value> cache_;
  int cache_key_ = -1;
  std::vector<uint8_t> src_;
  State state_;
};

}  // namespace mygo
