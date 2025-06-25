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
  void run_block(ast::NodePtr<ast::Block>& block);
  void run_if(ast::If* node);
  void run_declaration(ast::Declaration* node);
  void run_funcall(ast::Funcall* node);

      // global storage area
      int globals_;
  // stack pointer
  int sp_;
};

}  // namespace mygo
