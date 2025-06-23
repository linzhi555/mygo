#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "optional"
#include "value.h"

namespace mygo {

class Frame {
 public:
  Frame() : global_(nullptr) {}
  std::unique_ptr<Frame> MakeRoot() {
    auto res = std::make_unique<Frame>();
    return res;
  }

  std::optional<Value> Get(std::string_view key);
  void Set(std::string_view key, Value v);

 private:
  std::unordered_map<std::string, Value> vals_;

  class Frame* global_;
};

}  // namespace mygo
