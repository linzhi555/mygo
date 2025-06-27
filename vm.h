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
  Frame& scope() { return stack_.at(stack_.size() - 1); }

  VM() {
    stack_.push_back(Frame());
    globals_ = 0;
  };

  void debug_stack() {
    std::cout << "-----start stack-----" << std::endl;
    int i = 0;
    for (auto s : stack_) {
      std::cout << "---" << i << "---" << std::endl;
      s.debug();
      i++;
    }

    std::cout << "-----end start-----" << std::endl << std::endl;
  }

 private:
  void run_block(ast::NodePtr<ast::Block>& block);
  void run_if(ast::If* node);
  void run_declaration(ast::Declaration* node);
  void run_funcall(ast::Funcall* node);

  // global storage area
  int globals_;
};

}  // namespace mygo
