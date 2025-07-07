#pragma once

#include <string_view>
#include <vector>

#include "ast.h"
#include "frame.h"
#include "logging.h"
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
    LOG(INFO) << "-----start stack-----" << std::endl;
    int i = 0;
    for (auto s : stack_) {
      LOG(INFO) << "---" << i << "---" << std::endl;
      s.debug();
      i++;
    }

    LOG(INFO) << "-----end start-----" << std::endl << std::endl;
  }

 private:
  using ExitCode = std::string_view;
  static constexpr ExitCode NoExit = "NoExit";
  static constexpr ExitCode ExitNormal = "ExitNormal";
  static constexpr ExitCode ExitPanic = "ExitPanic";

  ExitCode exit_ = NoExit;

  void run_block(ast::NodePtr<ast::Block>& block);
  void run_if(ast::If* node);
  void run_declaration(ast::Declaration* node);
  void run_funcall(ast::Expr* node);

  // global storage area
  int globals_;
};

}  // namespace mygo
