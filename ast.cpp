#include "ast.h"

#include <memory>
#include <utility>

#include "iostream"
#include "logging.h"

namespace mygo {
namespace ast {

std::optional<NodePtr<Funcall>> Funcall::parse(TokenStream& stream) {
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

  return res;
};

std::optional<NodePtr<Root>> Root::parse(TokenStream& stream) {
  auto res = std::make_unique<Root>();
  while (true) {
    std::optional<NodePtr<Declaration>> decl;
    std::optional<NodePtr<Funcall>> fcall;

    if ((decl = Declaration::parse(stream))) {
      res->nodes_.push_back(std::move(decl.value()));
    } else if ((fcall = Funcall::parse(stream))) {
      res->nodes_.push_back(std::move(fcall.value()));
    } else {
      break;
    }

    EXPECT_TOKEN(stream, token::Type::Enl);
  };

  if (res->nodes_.empty()) return std::nullopt;
  return res;
};

std::optional<NodePtr<Declaration>> Declaration::parse(TokenStream& stream) {
  auto res = std::make_unique<Declaration>();
  LOG(INFO) << "before var " << stream.Peek().value() << " "
            << stream.Peek()->type() << " " << token::Type::Var << " "
            << std::endl;
  if (stream.Peek()->type() != token::Type::Var) return std::nullopt;
  stream.Next();

  LOG(INFO) << "before Symbol" << std::endl;
  if (!(stream.Peek()->type() == token::Type::Symbol)) return std::nullopt;
  res->var_name = stream.Peek()->str();
  stream.Next();

  LOG(INFO) << "before equal" << stream.Peek().value() << std::endl;
  if (!(stream.Peek()->type() == token::Type::Assign)) return std::nullopt;
  stream.Next();

  LOG(INFO) << "before expr" << stream.Peek().value() << std::endl;
  auto e = Expr::parse(stream);
  if (!e) return std::nullopt;
  res->expr = std::move(e.value());
  return res;
}

std::optional<NodePtr<Expr>> Expr::parse(TokenStream& stream) {
  auto expr0 = Expr::parse_l2(stream);
  if (!expr0) {
    return std::nullopt;
  }
  auto t = stream.Peek()->type();
  if (t != token::Type::Plus && t != token::Type::Sub) {
    return expr0;
  }

  auto multi = std::make_unique<Expr>();

  multi->exprs.push_back(std::move(expr0.value()));

  for (;;) {
    auto t = stream.Peek()->type();
    if (t == token::Type::Plus || t == token::Type::Sub) {
      stream.Next();
      auto newexpr = Expr::parse_l2(stream);
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

  return multi;
}

std::optional<NodePtr<Expr>> Expr::parse_l2(TokenStream& stream) {
  auto expr0 = Expr::parse_atomic(stream);
  if (!expr0) {
    return std::nullopt;
  } else {
    LOG(INFO) << "parse_atomic res" << expr0.value().get()->debug();
  }

  auto t = stream.Peek()->type();
  if (t != token::Type::Star && t != token::Type::Slash) {
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

  return multi;
}

std::optional<NodePtr<Expr>> Expr::parse_atomic(TokenStream& stream) {
  auto v = stream.Peek();
  if (!v) {
    return std::nullopt;
  }

  switch (v->type()) {
    case token::Type::Int:
    case token::Type::Str:
    case token::Type::Symbol: {
      stream.Next();
      return MakeAtomic(v.value());
    }
    default:
      return std::nullopt;
  }

  return std::nullopt;
};

}  // namespace ast
}  // namespace mygo
