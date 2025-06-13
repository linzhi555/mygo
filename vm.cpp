#include "vm.h"

#include <vector>

#include "ast.h"
#include "logging.h"

namespace mygo {

namespace vm {

std::optional<Value> std_eval(VM* vm, ast::NodePtr<ast::Expr>& expr) {
  if (expr->is_atomic) {
    switch (expr->v.type()) {
      case token::Type::Int:
        return Value::Make<int>(expr->v.i());

      case token::Type::Float:
        return Value::Make<float>(expr->v.f());

      case token::Type::Str:
        return Value::Make<std::string>(expr->v.str());
      case token::Type::Symbol: {
        std::string key = expr->v.str();
        if (vm->globals_.find(key) != vm->globals_.end()) {
          return vm->globals_[key];
        }
        break;
      }

      default:
        return std::nullopt;
    }
  }

  auto it = expr->exprs.begin();
  auto v1 = std_eval(vm, *it);
  LOG(INFO) << " v1 " << v1->ToString();
  if (v1->type != Value::Type::Int) return std::nullopt;
  int res = std::get<int>(v1->data);
  it++;

  for (auto op_it = expr->ops.begin();
       it != expr->exprs.end() && op_it != expr->ops.end(); it++, op_it++) {
    auto temp = std_eval(vm, *it);

    LOG(INFO) << " temp " << temp->ToString();
    if (temp && temp.value().type == Value::Type::Int) {
      int i_data = std::get<int>(temp->data);
      switch (*op_it) {
        case token::Type::Plus:
          res += i_data;
          break;
        case token::Type::Sub:
          res -= i_data;
          break;
        case token::Type::Star:
          res *= i_data;
          break;
        case token::Type::Slash:
          res /= i_data;
          break;
        default:
          return std::nullopt;
      }
    }
  }

  return Value::Make<int>(res);
}

void std_print(VM* vm, std::vector<ast::NodePtr<ast::Expr>>& args) {
  for (auto& arg : args) {
    std::cout << std_eval(vm, arg).value().ToString() << " ";
  }
  std::cout << std::endl;
}

void run_func(VM* vm, ast::Funcall* node) {
  if (node->func->v.str() == "print") {
    std_print(vm, node->arguments);
  } else {
    std::cout << "func is not defined " << node->func->v.str();
  }
}

void run_declaration(VM* vm, ast::Declaration* node) {
  vm->globals_[node->var_name] = std_eval(vm, node->expr).value();
}

void VM::run(ast::NodePtr<ast::Root>& root) {
  for (const std::unique_ptr<ast::Node>& node : root->nodes_) {
    // LOG(INFO) << "run root" << node->debug();
    switch (node->Type()) {
      case ast::Type::Funcall: {
        ast::Funcall* f = static_cast<ast::Funcall*>(node.get());
        LOG(INFO) << "run funcall" << f->debug();

        run_func(this, f);
        break;
      }
      case ast::Type::Declaration: {
        ast::Declaration* decl = static_cast<ast::Declaration*>(node.get());
        LOG(INFO) << "run Declaration" << decl->debug();
        run_declaration(this, decl);
        break;
      }
      default: {
        break;
      }
    }
  }
}
}  // namespace vm
}  // namespace mygo
