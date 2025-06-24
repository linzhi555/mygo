#pragma once

#include <vector>

#include "ast.h"
#include "frame.h"
namespace mygo {

class VM {
 public:
  void run(ast::NodePtr<ast::Root>& root);

  std::vector<Frame> stack_;

  Frame& global() { return stack_.at(0); }

  VM() {
    stack_.push_back(Frame());
    globals_ = 0;
    sp_ = 0;
  };

 private:
  // global storage area
  int globals_;
  // stack pointer
  int sp_;
};

}  // namespace mygo
