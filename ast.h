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
  Expr,
  For,
  If,
  Function,
  Declaration,
  Assignment,
  Funcall,
  Statement,
};

#define SKIP_TOKEN(S, T)                             \
  while (S.Peek() && S.Peek().value().type() == T) { \
    S.Next();                                        \
  }

#define SKIP_TOKEN_ONCE(S, T)                     \
  if (S.Peek() && S.Peek().value().type() == T) { \
    S.Next();                                     \
  }

#define EXPECT_TOKEN(S, T)                                              \
  if (!S.Peek())                                                        \
    return Err(S.loc(), std::string("expect token but get null") + #T); \
  if (S.Peek().value().type() != T) {                                   \
    return Err(S.loc(), std::string("expect token ") + #T);             \
  } else {                                                              \
    S.Next();                                                           \
  }

#define EXPECT_TOKEN_ERR(S, T, ERR)        \
  if (!S.Peek()) return Err(S.loc(), ERR); \
  if (S.Peek().value().type() != T) {      \
    return Err(S.loc(), ERR);              \
  } else {                                 \
    S.Next();                              \
  }

#define EXPECT_GET_TOKEN(S, T, ERR, RES)                                \
  if (!S.Peek())                                                        \
    return Err(S.loc(), std::string("expect token but get null") + #T); \
  if (S.Peek().value().type() != T) {                                   \
    return Err(S.loc(), std::string("expect token ") + #T);             \
  } else {                                                              \
    RES = S.Peek().value();                                             \
    S.Next();                                                           \
  }

template <typename T>
using NodePtr = std::unique_ptr<T>;

class Node {
 public:
  Loc start;
  Loc end;

  virtual enum Type Type() = 0;
  virtual std::string debug() = 0;
};

class Err {
 public:
  Err() = default;
  Err(Loc loc, std::string_view msg) { err_stack_.emplace_back(loc, msg); }
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
    res.value = std::move(v);
    return res;
  };

  Result() = default;

  Result(Err e) { err_ = e; }

  // static Result<T> err(std::string_view view) {
  //   Result res;
  //   res.err_ = Err(view);
  //   return res;
  // };

  NodePtr<T> value;
  Err err_;
  bool isOk() { return value.get() != nullptr; };
  bool isErr() { return !isOk(); }
  NodePtr<T>&& takeValue() { return std::move(value); }
  Err&& takeErr() { return std::move(err_); }
};

class Expr : public Node {
 public:
  bool is_atomic;
  token::Value v;
  std::vector<NodePtr<Expr>> exprs;
  std::vector<token::Type> ops;

  enum Type Type() override { return Type::Expr; };

  std::string debug() override {
    std::string s;
    if (is_atomic) {
      return v.debug();
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
    NodePtr<Expr> e = std::make_unique<Expr>();
    e->is_atomic = true;
    e->v = v;
    return e;
  }

  static Result<Expr> parse(TokenStream& stream);
  static Result<Expr> parse_with_greater(TokenStream& stream);
  static Result<Expr> parse_with_plus(TokenStream& stream);
  static Result<Expr> parse_with_star(TokenStream& stream);
  static Result<Expr> parse_atomic(TokenStream& stream);
};

class Declaration : public Node {
 public:
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

class Funcall : public Node {
 public:
  NodePtr<Expr> func;
  std::vector<NodePtr<Expr>> arguments;

  enum Type Type() override { return Type::Funcall; };

  static Result<Funcall> parse(TokenStream& stream);

  std::string debug() override {
    std::string s;
    s += "Funcall " + func->debug();
    for (auto& arg : arguments) {
      s += (std::string() + "\n" + "  " + arg->debug());
    }
    return s;
  }
};

class Block : public Node {
 public:
  std::vector<std::unique_ptr<Node>> nodes_;

  enum Type Type() override { return Type::Root; };
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

using Root = Block;

class Assignment : public Node {
 public:
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

class Function : public Node {
 public:
  std::string func_name;
  std::vector<std::pair<std::string, std::string>> args;
  std::vector<std::string> returns;
  NodePtr<Block> block;
  enum Type Type() override { return Type::Function; };
  std::string debug() override {
    std::string res;
    res += "Function:{\n";

    int i = 1;
    for (const auto& p : args) {
      res += "arg" + std::to_string(i) + " " + p.first + " " + p.second + "\n";
      i++;
    }

    res += block->debug();
    res += "}\n";
    return res;
  };
  static Result<Function> parse(TokenStream& stream);
};

class If : public Node {
 public:
  NodePtr<Expr> expr;
  NodePtr<Block> block;

  enum Type Type() override { return Type::If; };
  std::string debug() override {
    std::string res;
    res += "If:\n";
    res += expr->debug();
    res += "\n";
    res += block->debug();
    return res;
  };
  static Result<If> parse(TokenStream& stream);
};

class For : public Node {
  std::vector<NodePtr<Expr>> exprs;
  NodePtr<Block> block;
  enum Type Type() override { return Type::For; };
  std::string debug() override {
    std::string res;
    res += "For:\n";
    for (const auto& expr : exprs) {
      res += expr->debug();
      res += "\n";
    }
    res += block->debug();
    return res;
  };
};

}  // namespace ast
}  // namespace mygo
