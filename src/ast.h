#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "token.h"
#include "token_stream.h"
namespace mygo {
namespace ast {
enum class Type {
  Root,
  Block,
  Expr,
  For,
  If,
  Function,
  Declaration,
  Assignment,
  Funcall,
  Statement,
  Return,
};

template <typename T>
using NodePtr = std::unique_ptr<T>;

class Node {
 public:
  Loc start;
  Loc end;

  virtual enum Type Type() = 0;
  virtual std::string debug() = 0;
  virtual ~Node() {};
};

class Err {
 public:
  Err() = default;
  Err(Loc loc, std::string_view msg) { err_stack_.emplace_back(loc, msg); }

  // TODO: use static function rather to avodi function override
  Err(Err&& old, Loc loc, std::string_view msg) {
    err_stack_ = std::move(old.err_stack_);
    err_stack_.emplace_back(loc, msg);
  }

  int depth() { return err_stack_.size(); }

  using Item = std::pair<Loc, std::string>;
  std::string toString() {
    std::string res = "----backtrace-----\n";
    for (auto& s : err_stack_) {
      res += s.first.ToString();
      res += s.second;
      res += "\n";
    }
    return res;
  }
  const Item top() {
    assert(err_stack_.size() > 0);
    return err_stack_.at(err_stack_.size() - 1);
  }

 private:
  std::vector<Item> err_stack_;
};

template <typename T>
class Result {
 public:
  static Result<T> ok(NodePtr<T>&& v) {
    Result<T> res;
    res.value_ = std::move(v);
    return res;
  };

  Result() = default;

  Result(Err e) { err_ = e; }

  // static Result<T> err(std::string_view view) {
  //   Result res;
  //   res.err_ = Err(view);
  //   return res;
  // };

  NodePtr<T> value_;
  Err err_;
  bool isOk() { return value_.get() != nullptr; };
  bool isErr() { return !isOk(); }
  NodePtr<T>&& takeValue() { return std::move(value_); }
  Err&& takeErr() { return std::move(err_); }
};

enum class ExprType {
  TOKEN,
  OPS,
  FUNCALL,
};

class Expr : public Node {
 public:
  Expr() = delete;
  ~Expr() override = default;

  ExprType etype_;
  token::Value v;
  std::vector<NodePtr<Expr>> exprs;
  std::vector<token::Type> ops;

  enum Type Type() override { return Type::Expr; };
  Expr(ExprType t) : etype_(t) {};

  std::string debug() override {
    std::string s;
    if (etype_ == ExprType::TOKEN) {
      return v.debug();
    }

    if (etype_ == ExprType::FUNCALL) {
      s += "(funcall: ";
      for (auto& expr : exprs) {
        s += expr->debug();
      }
      s += ")";
      return s;
    }

    s += "(";

    size_t ops_len = ops.size();
    size_t exprs_len = exprs.size();
    for (size_t i = 0; i < exprs_len; i++) {
      s += exprs[i]->debug();
      if (i < ops_len) {
        s += token::debug_type(ops[i]);
      }
    }
    s += ")";
    return s;
  }

  static NodePtr<Expr> MakeAtomic(token::Value v) {
    NodePtr<Expr> e = std::make_unique<Expr>(ExprType::TOKEN);
    e->v = v;
    return e;
  }

  static Result<Expr> parse(TokenStream& stream);
  static Result<Expr> parse_with_greater(TokenStream& stream);
  static Result<Expr> parse_with_plus(TokenStream& stream);
  static Result<Expr> parse_with_star(TokenStream& stream);
  static Result<Expr> parse_atomic(TokenStream& stream);
  static Result<Expr> parse_token(TokenStream& stream);
  static Result<Expr> parse_funcall(TokenStream& stream);
};

class Declaration : public Node {
 public:
  Declaration() = default;
  ~Declaration() override = default;

  bool is_const;
  std::string var_name;
  NodePtr<Expr> expr;

  enum Type Type() override { return Type::Declaration; };

  static Result<Declaration> parse(TokenStream& stream);

  std::string debug() override {
    std::string s;
    s += "Declaration: ";
    s += var_name;
    s += " ";
    s += expr->debug();
    return s;
  }
};

class Assignment : public Node {
 public:
  Assignment() = default;
  ~Assignment() override = default;

  std::string var_name;
  NodePtr<Expr> expr;

  enum Type Type() override { return Type::Assignment; };
  std::string debug() override {
    std::string s;
    s += "Assignment:";
    s += var_name;
    s += " ";
    s += expr->debug();
    return s;
  };
  static Result<Assignment> parse(TokenStream& stream);
};

class Block : public Node {
 public:
  Block() = default;
  ~Block() override = default;
  std::vector<std::unique_ptr<Node>> nodes_;

  enum Type Type() override { return Type::Block; };
  std::string debug() override {
    std::string res;
    for (auto& node : nodes_) {
      res += node->debug();
      res += "\n";
    }
    return res;
  }

  static Result<Block> parse(TokenStream& stream);
};

class Root : public Node {
 public:
  Root() = default;
  ~Root() override = default;

  NodePtr<Block> block_;

  enum Type Type() override { return Type::Root; };
  std::string debug() override { return block_->debug(); }

  static Result<Root> parse(TokenStream& stream);
};

class Return : public Node {
 public:
  Return() = default;
  ~Return() override = default;
  enum Type Type() override { return Type::Return; };
  std::vector<NodePtr<Expr>> ret_exprs;
  std::string debug() override {
    std::string res;
    res += "Return\n";
    for (const NodePtr<Expr>& ret : ret_exprs) {
      res += ret->debug();
    }

    return res;
  }

  static Result<Return> parse(TokenStream& stream);
};

class Function : public Node {
 public:
  Function() = default;
  ~Function() override = default;

  std::string func_name;
  std::vector<std::pair<std::string, std::string>> args;
  std::vector<std::string> returns;
  NodePtr<Block> block;
  enum Type Type() override { return Type::Function; };
  std::string debug() override {
    std::string res;
    res += "Function:{\n";

    for (const auto& p : args) {
      res += "arg " + p.first + " " + p.second + "\n";
    }

    for (const auto& p : returns) {
      res += "return " + p + "\n";
    }

    res += block->debug();
    res += "}\n";
    return res;
  };
  static Result<Function> parse(TokenStream& stream);
};

class If : public Node {
 public:
  If() = default;
  ~If() override = default;

  using Branch = std::pair<NodePtr<Expr>, NodePtr<Block>>;
  std::vector<Branch> branches_;

  std::optional<NodePtr<Block>> tail_else_;

  enum Type Type() override { return Type::If; };
  std::string debug() override {
    std::string res;
    res += "If:\n";

    for (Branch& branch : branches_) {
      auto& [expr, block] = branch;
      res += expr->debug();
      res += "\n";
      res += block->debug();
    }

    if (tail_else_) {
      res += tail_else_.value()->debug();
    }

    return res;
  };
  static Result<If> parse(TokenStream& stream);
};

// TODO:need implement one statment and not state for
class For : public Node {
 public:
  For() = default;
  ~For() override = default;

  std::optional<NodePtr<Declaration>> init_stmt_;
  std::optional<NodePtr<Expr>> finish_cond_;
  std::optional<NodePtr<Assignment>> step_stmp_;

  NodePtr<Block> block_;
  enum Type Type() override { return Type::For; };
  std::string debug() override {
    std::string res;
    res += "For:\n";
    res += "init_stmt ";
    if (init_stmt_) {
      res += init_stmt_.value()->debug();
      res += "\n";
    }
    res += "finish_cond ";
    if (finish_cond_) {
      res += finish_cond_.value()->debug();
      res += "\n";
    }
    res += "step_stmp_ ";
    if (step_stmp_) {
      res += step_stmp_.value()->debug();
      res += "\n";
    }

    res += block_->debug();
    return res;
  };

  static Result<For> parse(TokenStream& stream);
};

// TODO: need implement struct node
class Struct : public Node {};

}  // namespace ast
}  // namespace mygo
