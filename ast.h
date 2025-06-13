#pragma once

#include <memory>
#include <optional>
#include <string>
// #include <utility>
#include <vector>

#include "token.h"
#include "token_stream.h"
namespace mygo {
namespace ast {
enum class Type {
  Root,
  Expr,
  Declaration,
  Assignment,
  Funcall,
  Statement,
};

//#define SKIP_SPACE(S)                                                 \
//  while (S.Peek() && S.Peek().value().type() == token::Type::Space) { \
//    S.Next();                                                         \
//  }

#define EXPECT_TOKEN(S, T)            \
  if (S.Peek().value().type() != T) { \
    return std::nullopt;              \
  } else {                            \
    S.Next();                         \
  }

template <typename T>
using NodePtr = std::unique_ptr<T>;

class Node {
 public:
  virtual enum Type Type() = 0;
  virtual std::string debug() = 0;
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

  static NodePtr<Expr> CombineExpr(token::Type op, std::unique_ptr<Expr> e1,
                                   std::unique_ptr<Expr> e2) {
    NodePtr<Expr> e = std::make_unique<Expr>();
    e->ops.push_back(op);
    e->exprs.push_back(std::move(e1));
    e->exprs.push_back(std::move(e2));
    return e;
  }

  static std::optional<NodePtr<Expr>> parse(TokenStream& stream);
  static std::optional<NodePtr<Expr>> parse_l2(TokenStream& stream);
  static std::optional<NodePtr<Expr>> parse_atomic(TokenStream& stream);
};

class Declaration : public Node {
 public:
  bool is_const;
  std::string var_name;
  NodePtr<Expr> expr;

  enum Type Type() override { return Type::Declaration; };

  static std::optional<std::unique_ptr<Declaration>> parse(TokenStream& stream);

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

  static std::optional<NodePtr<Funcall>> parse(TokenStream& stream);

  std::string debug() override {
    std::string s;
    s += "Funcall " + func->debug();
    for (auto& arg : arguments) {
      s += (std::string() + "\n" + "  " + arg->debug());
    }
    return s;
  }
};

class Root : public Node {
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

  static std::optional<std::unique_ptr<Root>> parse(TokenStream& stream);
};

struct Assignment {};
struct Function {};

}  // namespace ast
}  // namespace mygo
