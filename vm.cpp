#include "vm.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

#include "ast.h"

// #include "logging.h"

namespace mygo {

namespace {

bool is_logic_operator(token::Type t) {
  switch (t) {
    case token::Type::Greater:
    case token::Type::Less:
    case token::Type::Equal:
      return true;
    default:
      return false;
  }
}

bool to_number(Value v, float& data) {
  // match start
  if (v.type == Value::Int) {
    data = std::get<int>(v.data);
    return true;
  }

  if (v.type == Value::Float) {
    data = std::get<float>(v.data);
    return true;
  }
  // match end

  return false;
}

std::optional<Value> operator_greater(Value v1, Value v2) {
  float f1, f2;
  if (to_number(v1, f1) && to_number(v2, f2)) return Value::Make(f1 > f2);
  return std::nullopt;
}

std::optional<Value> operator_less(Value v1, Value v2) {
  float f1, f2;
  if (to_number(v1, f1) && to_number(v2, f2)) return Value::Make(f1 < f2);
  return std::nullopt;
}

std::optional<Value> operator_equal(Value v1, Value v2) {
  float f1, f2;
  if (to_number(v1, f1) && to_number(v2, f2)) return Value::Make(f1 == f2);
  return std::nullopt;
}

}  // namespace

std::optional<Value> std_eval(VM* vm, ast::NodePtr<ast::Expr>& expr) {
  if (expr->is_atomic) {
    switch (expr->v.type()) {
      case token::Type::Int:
        return Value::Make<int>(expr->v.i());

      case token::Type::Float:
        return Value::Make<float>(expr->v.f());

      case token::Type::True:
        return Value::Make<bool>(true);

      case token::Type::False:
        return Value::Make<bool>(false);

      case token::Type::Str:
        return Value::Make<std::string>(expr->v.str());

      case token::Type::Symbol: {
        std::string key = expr->v.str();
        auto v = vm->scope().Get(key);
        if (v) {
          return v;
        }
        break;
      }

      default:
        return std::nullopt;
    }
  }

  // logic operator condition
  do {
    if (expr->ops.size() != 1) break;
    token::Type t = expr->ops.at(0);
    if (!is_logic_operator(t)) break;

    assert(expr->exprs.size() == 2);

    auto v1 = std_eval(vm, expr->exprs.at(0));
    auto v2 = std_eval(vm, expr->exprs.at(1));

    if (!v1 || !v2) return std::nullopt;
    switch (t) {
      case token::Type::Greater:
        return operator_greater(v1.value(), v2.value());
      case token::Type::Less:
        return operator_less(v1.value(), v2.value());
      case token::Type::Equal:
        return operator_equal(v1.value(), v2.value());
      default:
        assert("should not reach here" == nullptr);
    }
  } while (false);

  auto it = expr->exprs.begin();
  auto v1 = std_eval(vm, *it);
  if (v1->type != Value::Int) return std::nullopt;
  int res = std::get<int>(v1->data);
  it++;

  for (auto op_it = expr->ops.begin();
       it != expr->exprs.end() && op_it != expr->ops.end(); it++, op_it++) {
    auto temp = std_eval(vm, *it);

    if (temp && temp.value().type == Value::Int) {
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

void VM::run_funcall(ast::Funcall* node) {
  if (node->func->v.str() == "print") {
    std_print(this, node->arguments);
  } else if (auto v = global().Get(node->func->v.str())) {
    // simulate push new stack frame and do funcall
    debug_stack();
    stack_.push_back(Frame(&global()));
    run_block(v->As<ast::Function*>()->block);
    debug_stack();
    stack_.pop_back();
    debug_stack();
  } else {
    std::cerr << "func is not defined " << node->func->v.str() << std::endl;
  }
}

void VM::run_declaration(ast::Declaration* node) {
  scope().Set(node->var_name, std_eval(this, node->expr).value());
}

void VM::run_if(ast::If* node) {
  std::optional<Value> v = std_eval(this, node->expr);
  if (v && v->type == Value::Bool && std::get<bool>(v->data)) {
    run_block(node->block);
  }
}

void VM::run_block(ast::NodePtr<ast::Block>& block) {
  for (const std::unique_ptr<ast::Node>& node : block->nodes_) {
    switch (node->Type()) {
      case ast::Type::Funcall: {
        ast::Funcall* f = static_cast<ast::Funcall*>(node.get());

        run_funcall(f);
        break;
      }

      case ast::Type::Declaration: {
        ast::Declaration* decl = static_cast<ast::Declaration*>(node.get());
        run_declaration(decl);
        break;
      }

      case ast::Type::Function: {
        ast::Function* func = static_cast<ast::Function*>(node.get());
        global().Set(func->func_name, Value::Make(func));
        break;
      }

      case ast::Type::If: {
        ast::If* decl = static_cast<ast::If*>(node.get());
        run_if(decl);
        break;
      }

      default: {
        break;
      }
    }
  }
}

void VM::run(ast::NodePtr<ast::Root>& root) { run_block(root); }
}  // namespace mygo
