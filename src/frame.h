#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "logging.h"
#include "optional"
#include "value.h"

namespace mygo {

class Frame {
 public:
  Frame() : global_(nullptr) {}
  Frame(Frame* global) : global_(global) {}
  std::unique_ptr<Frame> MakeRoot() {
    auto res = std::make_unique<Frame>();
    return res;
  }

  std::optional<Value> Get(std::string_view key);
  void Set(std::string_view key, Value v);

  void debug() {
    for (auto k : vals_) {
      LOG(INFO) << k.first << " " << k.second.ToString() << std::endl;
    }
  }

 private:
  std::unordered_map<std::string, Value> vals_;

  class Frame* global_;
};

}  // namespace mygo
