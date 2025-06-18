#pragma once

#include <memory>
#include <optional>
#include <string>
// #include <utility>
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

#define EXPECT_TOKEN(S, T)            \
  if (!S.Peek()) return std::nullopt; \
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
 // virtual std::pair<token::CharPtr, token::CharPtr> Range() = 0;
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
  static std::optional<NodePtr<Expr>> parse_with_greater(TokenStream& stream);
  static std::optional<NodePtr<Expr>> parse_with_plus(TokenStream& stream);
  static std::optional<NodePtr<Expr>> parse_with_star(TokenStream& stream);
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

  static std::optional<std::unique_ptr<Block>> parse(TokenStream& stream);
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
  static std::optional<NodePtr<Assignment>> parse(TokenStream& stream);
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
    res += "Function:\n";
    res += block->debug();
    return res;
  };
  static std::optional<NodePtr<Function>> parse(TokenStream& stream);
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
  static std::optional<NodePtr<If>> parse(TokenStream& stream);
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
