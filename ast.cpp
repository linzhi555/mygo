#include "ast.h"

#include <memory>
#include <utility>

namespace mygo {
namespace ast {

std::optional<NodePtr<Funcall>> Funcall::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Funcall>();

  std::optional<NodePtr<Expr>> f = Expr::parse(stream);
  if (!f) return std::nullopt;
  res->func = std::move(f.value());

  EXPECT_TOKEN(stream, token::Type::LParent);
  while (true) {
    auto expr = Expr::parse(stream);
    if (expr) {
      res->arguments.emplace_back(std::move(expr.value()));
    } else {
      break;
    }

    if (stream.Peek() && stream.Peek()->type() == token::Type::Comma) {
      stream.Next();
    } else {
      break;
    }
  }

  EXPECT_TOKEN(stream, token::Type::RParent);

  guard.Cancel();
  return res;
};

std::optional<NodePtr<Block>> Block::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Block>();
  while (true) {
    std::optional<NodePtr<Declaration>> decl;
    std::optional<NodePtr<Funcall>> fcall;
    std::optional<NodePtr<If>> if_node;
    std::optional<NodePtr<Function>> func_node;

    SKIP_TOKEN(stream, token::Type::Enl);

    if ((decl = Declaration::parse(stream))) {
      res->nodes_.push_back(std::move(decl.value()));
    } else if ((fcall = Funcall::parse(stream))) {
      res->nodes_.push_back(std::move(fcall.value()));
    } else if ((if_node = If::parse(stream))) {
      res->nodes_.push_back(std::move(if_node.value()));

    } else if ((func_node = Function::parse(stream))) {
      res->nodes_.push_back(std::move(func_node.value()));

    } else {
      break;
    }

    EXPECT_TOKEN(stream, token::Type::Enl);
  };

  if (res->nodes_.empty()) return std::nullopt;

  guard.Cancel();
  return res;
};

std::optional<NodePtr<Declaration>> Declaration::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Declaration>();
  auto var_mutablity = stream.Peek();
  if (var_mutablity->type() != token::Type::Var) return std::nullopt;
  stream.Next();

  if (!(stream.Peek()->type() == token::Type::Symbol)) return std::nullopt;
  res->var_name = stream.Peek()->str();
  stream.Next();

  if (!(stream.Peek()->type() == token::Type::Assign)) return std::nullopt;
  stream.Next();

  auto e = Expr::parse(stream);
  if (!e) return std::nullopt;
  res->expr = std::move(e.value());

  res->start = var_mutablity->start;
  res->end = res->expr->end;
  guard.Cancel();
  return res;
}

std::optional<NodePtr<Expr>> Expr::parse(TokenStream& stream) {
  return Expr::parse_with_greater(stream);
}

std::optional<NodePtr<Expr>> Expr::parse_with_greater(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto expr0 = Expr::parse_with_plus(stream);
  if (!expr0) return std::nullopt;

  auto t = stream.Peek()->type();
  if (t != token::Type::Greater && t != token::Type::Less &&
      t != token::Type::Equal) {
    guard.Cancel();
    return expr0;
  }

  stream.Next();

  auto multi = std::make_unique<Expr>();

  auto expr1 = Expr::parse_with_plus(stream);
  if (!expr1) return std::nullopt;

  multi->ops.push_back(t);
  multi->exprs.push_back(std::move(expr0.value()));
  multi->exprs.push_back(std::move(expr1.value()));

  guard.Cancel();
  return multi;
}

std::optional<NodePtr<Expr>> Expr::parse_with_plus(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto expr0 = Expr::parse_with_star(stream);
  if (!expr0) {
    return std::nullopt;
  }
  auto t = stream.Peek()->type();
  if (t != token::Type::Plus && t != token::Type::Sub) {
    guard.Cancel();
    return expr0;
  }

  auto multi = std::make_unique<Expr>();

  multi->exprs.push_back(std::move(expr0.value()));

  for (;;) {
    auto t = stream.Peek()->type();
    if (t == token::Type::Plus || t == token::Type::Sub) {
      stream.Next();
      auto newexpr = Expr::parse_with_star(stream);
      if (newexpr) {
        multi->ops.push_back(t);
        multi->exprs.push_back(std::move(newexpr.value()));
      } else {
        return std::nullopt;
      }
    } else {
      break;
    }
  }

  guard.Cancel();
  return multi;
}

std::optional<NodePtr<Expr>> Expr::parse_with_star(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto expr0 = Expr::parse_atomic(stream);
  if (!expr0) {
    return std::nullopt;
  } else {
  }

  auto t = stream.Peek()->type();
  if (t != token::Type::Star && t != token::Type::Slash) {
    guard.Cancel();
    return expr0;
  }

  auto multi = std::make_unique<Expr>();

  multi->exprs.push_back(std::move(expr0.value()));

  for (;;) {
    auto t = stream.Peek()->type();
    if (t == token::Type::Star || t == token::Type::Slash) {
      stream.Next();
      auto newexpr = Expr::parse_atomic(stream);
      if (newexpr) {
        multi->ops.push_back(t);
        multi->exprs.push_back(std::move(newexpr.value()));
      } else {
        return std::nullopt;
      }
    } else {
      break;
    }
  }

  guard.Cancel();
  return multi;
}

std::optional<NodePtr<Expr>> Expr::parse_atomic(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto v = stream.Peek();
  if (!v) {
    return std::nullopt;
  }

  switch (v->type()) {
    case token::Type::Int:
    case token::Type::Float:
    case token::Type::Str:
    case token::Type::True:
    case token::Type::False:
    case token::Type::Symbol: {
      stream.Next();

      NodePtr<Expr> res = MakeAtomic(v.value());
      res->start = v->start;
      res->end = v->end;
      guard.Cancel();
      return res;
    }

    default:
      return std::nullopt;
  }

  return std::nullopt;
};

std::optional<NodePtr<Function>> Function::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::Func);
  std::string func_name;
  if (!(stream.Peek()->type() == token::Type::Symbol)) return std::nullopt;
  func_name = stream.Peek()->str();
  stream.Next();

  EXPECT_TOKEN(stream, token::Type::LParent);

  EXPECT_TOKEN(stream, token::Type::RParent);

  EXPECT_TOKEN(stream, token::Type::LBrace);

  SKIP_TOKEN(stream, token::Type::Enl);
  std::optional<NodePtr<Block>> block = Block::parse(stream);
  if (!block) return std::nullopt;
  EXPECT_TOKEN(stream, token::Type::RBrace);

  NodePtr<Function> func_node = std::make_unique<Function>();
  func_node->func_name = func_name;

  func_node->block = std::move(block.value());

  guard.Cancel();
  return func_node;
}

std::optional<NodePtr<If>> If::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::If);
  EXPECT_TOKEN(stream, token::Type::LParent);
  std::optional<NodePtr<Expr>> expr = Expr::parse(stream);
  if (!expr) return std::nullopt;
  EXPECT_TOKEN(stream, token::Type::RParent);

  EXPECT_TOKEN(stream, token::Type::LBrace);

  SKIP_TOKEN(stream, token::Type::Enl);
  std::optional<NodePtr<Block>> block = Block::parse(stream);
  if (!block) return std::nullopt;
  EXPECT_TOKEN(stream, token::Type::RBrace);

  NodePtr<If> if_node = std::make_unique<If>();

  if_node->expr = std::move(expr.value());
  if_node->block = std::move(block.value());

  guard.Cancel();
  return if_node;
}

}  // namespace ast
}  // namespace mygo
