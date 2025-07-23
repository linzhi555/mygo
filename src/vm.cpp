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
  if (v.type == Value::INT) {
    data = std::get<int>(v.data);
    return true;
  }

  if (v.type == Value::FLOAT) {
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

void std_print(VM* vm, std::vector<ast::Expr*>& args);

VM::VM() {
  stack_.push_back(Frame());

  // install builtins
  stack_.at(0).Set("print", Value::Make(Func(std_print)));

  stack_.at(0).Set("args", Value::Make(Func([](VM*, std::vector<ast::Expr*>&) {
                     std::cout << "helloworld" << std::endl;
                   })));
  //
};

std::optional<Value> VM::std_eval(ast::Expr* expr) {
  if (expr->etype_ == ast::ExprType::FUNCALL) {
    run_funcall(expr);
    if (exit_ == ExitNormal) {
      exit_ = NoExit;
      return ret_;
    }

    return std::nullopt;
  }

  if (expr->etype_ == ast::ExprType::TOKEN) {
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
        auto v = scope().Get(key);
        if (v) {
          return v;
        } else {
          return std::nullopt;
        }
      }

      default:
        return std::nullopt;
    }
  }

  // TODO: need support ! and && and ||
  do {
    if (expr->ops.size() != 1) break;
    token::Type t = expr->ops.at(0);
    if (!is_logic_operator(t)) break;

    assert(expr->exprs.size() == 2);

    auto v1 = std_eval(expr->exprs.at(0).get());
    auto v2 = std_eval(expr->exprs.at(1).get());

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

  auto expr_it = expr->exprs.begin();
  std::optional<Value> v1 = std_eval(expr_it->get());

  if (!v1) {
    return std::nullopt;
  }
  expr_it++;
  auto op_it = expr->ops.begin();

  Value res = v1.value();

  // single op expr
  if (expr->ops.size() == 1 && expr->exprs.size() == 1) {
    return res.Operator(*op_it);
  }

  for (; expr_it != expr->exprs.end() && op_it != expr->ops.end();
       expr_it++, op_it++) {
    std::optional<Value> temp = std_eval(expr_it->get());
    if (!temp) return std::nullopt;

    std::optional<Value> res_temp = res.Operator(*op_it, temp.value());
    if (!res_temp) return std::nullopt;
    res = res_temp.value();
  }

  return res;
}

void std_print(VM* vm, std::vector<ast::Expr*>& args) {
  for (ast::Expr* arg : args) {
    auto v = vm->std_eval(arg);
    std::cout << (v.has_value() ? v->ToString() : "undefined") << " ";
  }
  std::cout << std::endl;
}

void VM::run_funcall(ast::Expr* node) {
  assert(node->etype_ == ast::ExprType::FUNCALL);
  assert(node->exprs.size() >= 1);
  ast::NodePtr<ast::Expr>& f = node->exprs.at(0);
  std::optional<Value> func_maybe = global().Get(f->v.str());
  if (!func_maybe.has_value()) {
    exit_ = ExitPanic;
    return;
  }
  if (func_maybe.value().type != Value::FUNC) {
    exit_ = ExitPanic;
    return;
  };

  Func func = func_maybe.value().As<Func>();

  if (func.isBuiltin()) {
    std::vector<ast::Expr*> args;
    for (auto it = node->exprs.begin() + 1; it != node->exprs.end(); it++) {
      args.push_back(it->get());
    }
    func.AsBuiltin()(this, args);
  } else {
    // simulate push new stack frame and do funcall
    //
    auto func = func_maybe->As<Func>().AsUserDef();

    assert(node->exprs.size() - 1 == func->args.size());

    int i = 0;

    auto newframe = Frame(&global());

    for (auto it = node->exprs.begin() + 1; it != node->exprs.end(); it++) {
      std::string arg_name = func->args.at(i).first;
      std::optional<Value> a = std_eval(it->get());
      assert(a.has_value());
      newframe.Set(arg_name, a.value());
      i++;
    }

    stack_.push_back(newframe);
    run_block(func->block);
    stack_.pop_back();
  }
}

void VM::run_declaration(ast::Declaration* node) {
  std::optional<Value> v = std_eval(node->expr.get());
  if (v) {
    scope().Set(node->var_name, v.value());
  } else {
    this->exit_ = ExitPanic;
  }
}

void VM::run_assignment(ast::Assignment* node) {
  if (scope().Get(node->var_name) == std::nullopt) {
    this->exit_ = ExitPanic;
    return;
  }

  std::optional<Value> v = std_eval(node->expr.get());
  if (v) {
    scope().Set(node->var_name, v.value());
  } else {
    this->exit_ = ExitPanic;
  }
}

void VM::run_if(ast::If* node) {
  for (ast::If::Branch& branch : node->branches_) {
    auto& [expr, block] = branch;
    std::optional<Value> v = std_eval(expr.get());
    if (v && v->type == Value::BOOL && std::get<bool>(v->data)) {
      run_block(block);
      return;
    }
  }
  if (node->tail_else_) {
    run_block(node->tail_else_.value());
  }
}

void VM::run_for(ast::For* node) {
  if (node->init_stmt_) {
    run_declaration(node->init_stmt_->get());
  }

  while (true) {
    if (this->exit_ == ExitNormal || this->exit_ == ExitPanic) {
      return;
    }

    if (node->finish_cond_) {
      std::optional<Value> v = std_eval(node->finish_cond_.value().get());
      if (!v.has_value() || v->type != Value::BOOL ||
          !std::get<bool>(v->data)) {
        break;
      }
    }
    run_block(node->block_);

    if (node->step_stmp_) {
      run_assignment(node->step_stmp_->get());
    }
  }
}

// TODO: need support scope
void VM::run_block(ast::NodePtr<ast::Block>& block) {
  for (const std::unique_ptr<ast::Node>& node : block->nodes_) {
    if (this->exit_ == ExitNormal || this->exit_ == ExitPanic) {
      return;
    }

    switch (node->Type()) {
      case ast::Type::Expr: {
        ast::Expr* expr = static_cast<ast::Expr*>(node.get());
        if (expr->etype_ == ast::ExprType::FUNCALL) {
          std_eval(expr);
          if (exit_ == ExitNormal) {
            exit_ = NoExit;
          }
        }
        break;
      }

      case ast::Type::Declaration: {
        ast::Declaration* decl = static_cast<ast::Declaration*>(node.get());
        run_declaration(decl);
        break;
      }

      case ast::Type::Assignment: {
        ast::Assignment* asgn = static_cast<ast::Assignment*>(node.get());
        run_assignment(asgn);
        break;
      }

      case ast::Type::Function: {
        ast::Function* func = static_cast<ast::Function*>(node.get());
        global().Set(func->func_name, Value::Make(Func(func)));
        break;
      }

      case ast::Type::If: {
        ast::If* decl = static_cast<ast::If*>(node.get());
        run_if(decl);
        break;
      }

      case ast::Type::Return: {
        ast::Return* ret = static_cast<ast::Return*>(node.get());
        for (const auto& expr : ret->ret_exprs) {
          std::optional<Value> v = std_eval(expr.get());
          if (v) {
            ret_ = v.value();
          } else {
            exit_ = ExitPanic;
            return;
          }
        }

        this->exit_ = ExitNormal;

        break;
      }

      case ast::Type::For: {
        ast::For* for_node = static_cast<ast::For*>(node.get());
        run_for(for_node);
        break;
      }

      default: {
        assert("should not reach here" && false);
        this->exit_ = ExitPanic;
        break;
      }
    }
  }
  this->exit_ = ExitNormal;
}

void VM::run(ast::NodePtr<ast::Root>& root) { run_block(root->block_); }
}  // namespace mygo
