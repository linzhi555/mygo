#include <memory>

#include "ast.h"
namespace mygo {

class Backend {
  virtual void run(ast::NodePtr<ast::Root>& root) = 0;
};

class CBackend : public Backend {
  void run(ast::NodePtr<ast::Root>& root) override;
};

class ByteCodeBackend : public Backend {
  void run(ast::NodePtr<ast::Root>& root) override;
};

}  // namespace mygo
