#include "logging.h"

#include <iostream>
#include <streambuf>
// 自定义 streambuf：所有输出操作均无效
class NullBuffer : public std::streambuf {
 public:
  int overflow(int c) override {
    // 丢弃所有字符
    return c;
  }
};

// 自定义 ostream：绑定到 NullBuffer
class NullStream : public std::ostream {
 public:
  NullStream() : std::ostream(&null_buffer_) {}

 private:
  NullBuffer null_buffer_;
};

// 全局黑洞流实例
NullStream null_stream;

Serverity debug_level = WARNING;

void set_debug_level(Serverity s) { debug_level = s; }

std::ostream& log_stream(Serverity s) {
  if (debug_level > s) return null_stream;

  std::cout << std::endl << "LEVEL" << s << " ";
  return std::cout;
}
