#include "frame.h"

#include <optional>
#include <string>

namespace mygo {

//TODO: need support true scope
std::optional<Value> Frame::Get(std::string_view key) {
  std::string k(key);
  if (vals_.find(k) == vals_.end()) {
    return std::nullopt;
  } else {
    return vals_.at(k);
  }

  return std::nullopt;
}

void Frame::Set(std::string_view key, Value v) {
  std::string k(key);
  vals_[k] = v;
}

}  // namespace mygo
