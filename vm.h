#pragma once

#include "ast.h"
#include "frame.h"
namespace mygo {

class VM {
 public:
  void run(ast::NodePtr<ast::Root>& root);
  Frame globals;
};

}  // namespace mygo
