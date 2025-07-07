#include "ast.h"

#include <cassert>
#include <memory>
#include <string>
#include <utility>

#include "logging.h"

namespace mygo {
namespace ast {

Result<Return> Return::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);
  auto res = std::make_unique<Return>();
  EXPECT_TOKEN(stream, token::Type::Return);

  for (int i = 0;; i++) {
    assert(i < 100);
    Result e = Expr::parse(stream);
    if (e.isOk()) {
      res->ret_exprs.push_back(std::move(e.takeValue()));
    } else {
      break;
    }
  }

  guard.finish(res->start, res->end);
  return Result<Return>::ok(std::move(res));
}

Result<Block> Block::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Block>();
  for (int i = 0; i < 100000; i++) {
    SKIP_TOKEN(stream, token::Type::Enl);
    {
      Result<Declaration> decl = Declaration::parse(stream);
      if (decl.isOk()) {
        res->nodes_.push_back(decl.takeValue());
        continue;
      }
    }

    {
      Result<Expr> expr = Expr::parse(stream);
      if (expr.isOk()) {
        res->nodes_.push_back(expr.takeValue());
        continue;
      }
    }

    {
      Result<If> if_node = If::parse(stream);
      if (if_node.isOk()) {
        res->nodes_.push_back(if_node.takeValue());
        continue;
      }

      Err e = if_node.takeErr();
      if (e.top().first != stream.loc()) {
        return Err(if_node.takeErr(), stream.loc(),
                   std::string("parse if error"));
      }
    }

    {
      Result<Function> func_node = Function::parse(stream);
      if (func_node.isOk()) {
        res->nodes_.push_back(func_node.takeValue());
        continue;
      }
      Err e = func_node.takeErr();
      if (e.top().first != stream.loc())
        return Err(std::move(e), stream.loc(), std::string("parse func error"));
    }

    {
      Result<Return> node = Return::parse(stream);
      if (node.isOk()) {
        res->nodes_.push_back(node.takeValue());
        continue;
      }
      Err e = node.takeErr();
      if (e.top().first != stream.loc())
        return Err(std::move(e), stream.loc(),
                   std::string("parse return error"));
    }

    break;
  };

  guard.finish(res->start, res->end);
  return Result<Block>::ok(std::move(res));
};

Result<Declaration> Declaration::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Declaration>();
  EXPECT_TOKEN(stream, token::Type::Var)

  token::Value v;
  EXPECT_GET_TOKEN(stream, token::Type::Symbol, "expect symbol", v);
  res->var_name = v.str();

  EXPECT_TOKEN(stream, token::Type::Assign)

  Result<Expr> e = Expr::parse(stream);
  if (e.isErr()) return (Err(e.takeErr(), stream.loc(), "parse Expr error"));
  res->expr = e.takeValue();

  guard.finish(res->start, res->end);
  return Result<Declaration>::ok(std::move(res));
}

Result<Expr> Expr::parse(TokenStream& stream) {
  return Expr::parse_with_greater(stream);
}

Result<Expr> Expr::parse_with_greater(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  Result<Expr> temp = Expr::parse_with_plus(stream);
  if (temp.isErr()) return Err(stream.loc(), "parse expr error");
  NodePtr<Expr> expr0 = temp.takeValue();

  auto t = stream.Peek()->type();
  if (t != token::Type::Greater && t != token::Type::Less &&
      t != token::Type::Equal) {
    guard.finish(expr0->start, expr0->end);
    return Result<Expr>::ok(std::move(expr0));
  }

  stream.Next();

  auto multi = std::make_unique<Expr>(ExprType::OPS);

  auto expr1_res = Expr::parse_with_plus(stream);
  if (expr1_res.isErr()) return Err(stream.loc(), "parse expr error");

  multi->ops.push_back(t);
  multi->exprs.push_back(std::move(expr0));
  multi->exprs.push_back(expr1_res.takeValue());

  guard.finish(multi->start, multi->end);
  return Result<Expr>::ok(std::move(multi));
}

Result<Expr> Expr::parse_with_plus(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  Result<Expr> expr0_res = Expr::parse_with_star(stream);
  if (expr0_res.isErr()) {
    return Err(stream.loc(), "parse expr with error");
  }
  auto t = stream.Peek()->type();
  if (t != token::Type::Plus && t != token::Type::Sub) {
    guard.finish(expr0_res.value->start, expr0_res.value->end);
    return expr0_res;
  }

  auto multi = std::make_unique<Expr>(ExprType::OPS);

  multi->exprs.push_back(expr0_res.takeValue());

  for (;;) {
    auto t = stream.Peek()->type();
    if (t == token::Type::Plus || t == token::Type::Sub) {
      stream.Next();
      auto newexpr = Expr::parse_with_star(stream);
      if (newexpr.isOk()) {
        multi->ops.push_back(t);
        multi->exprs.push_back(newexpr.takeValue());
      } else {
        return Err(stream.loc(), "parse expr error");
      }
    } else {
      break;
    }
  }

  guard.finish(multi->start, multi->end);
  return Result<Expr>::ok(std::move(multi));
}

Result<Expr> Expr::parse_with_star(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto expr0_res = Expr::parse_atomic(stream);
  if (expr0_res.isErr()) {
    return Err(stream.loc(), "parse atomic expr error");
  }

  auto t = stream.Peek()->type();
  if (t != token::Type::Star && t != token::Type::Slash) {
    guard.finish(expr0_res.value->start, expr0_res.value->end);
    return expr0_res;
  }

  auto multi = std::make_unique<Expr>(ExprType::OPS);

  multi->exprs.push_back(expr0_res.takeValue());

  for (;;) {
    auto t = stream.Peek()->type();
    if (t == token::Type::Star || t == token::Type::Slash) {
      stream.Next();
      auto newexpr = Expr::parse_atomic(stream);
      if (newexpr.isOk()) {
        multi->ops.push_back(t);
        multi->exprs.push_back(newexpr.takeValue());
      } else {
        return Err(stream.loc(), "parse expr with star error");
      }
    } else {
      break;
    }
  }

  guard.finish(multi->start, multi->end);
  return Result<Expr>::ok(std::move(multi));
}

Result<Expr> Expr::parse_atomic(TokenStream& stream) {
  Result<Expr> fc = Expr::parse_funcall(stream);
  if (fc.isOk()) {
    return fc;
  }

  Result<Expr> tk = Expr::parse_token(stream);
  if (tk.isOk()) {
    return tk;
  }

  return Err(stream.loc(), "parse atomic error");
}

Result<Expr> Expr::parse_token(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto v = stream.Peek();
  if (!v) {
    return Err(stream.loc(), "parse tk error");
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
      guard.finish(res->start, res->end);
      return Result<Expr>::ok(std::move(res));
    }

    default:
      break;
  }
  return Err(stream.loc(), "parse tk error");
};

Result<Expr> Expr::parse_funcall(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Expr>(ExprType::FUNCALL);

  Result<Expr> f = Expr::parse_token(stream);
  if (f.isErr()) return Err(stream.loc(), "expect err");
  res->exprs.emplace_back(f.takeValue());

  EXPECT_TOKEN(stream, token::Type::LParent);

  while (true) {
    auto expr = Expr::parse(stream);
    if (expr.isOk()) {
      res->exprs.emplace_back(expr.takeValue());
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

  guard.finish(res->start, res->end);
  return Result<Expr>::ok(std::move(res));
};

Result<Function> Function::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::Func);

  NodePtr<Function> func_node = std::make_unique<Function>();
  std::string func_name;
  if (!(stream.Peek()->type() == token::Type::Symbol))
    Err(stream.loc(), "expect symbol");
  func_name = stream.Peek()->str();
  stream.Next();

  EXPECT_TOKEN(stream, token::Type::LParent);

  for (int i = 0; i < 1000; i++) {
    std::string val_name, val_t;

    token::Value temp;

    if (stream.Peek()->type() != token::Type::Symbol) {
      LOG(INFO) << "finish  func arguments parse " << func_name << std::endl;
      break;
    }
    EXPECT_GET_TOKEN(stream, token::Type::Symbol, "need symbol", temp);
    val_name = temp.str();
    EXPECT_GET_TOKEN(stream, token::Type::Symbol, "need symbol", temp);
    val_t = temp.str();

    SKIP_TOKEN_ONCE(stream, token::Type::Comma);

    func_node->args.push_back({val_name, val_t});
  }

  EXPECT_TOKEN_ERR(stream, token::Type::RParent,
                   (std::string("expect ) but get") + stream.Peek()->debug()));

  // parse returns, allow max 99 return
  for (int i = 0; i < 100; i++) {
    assert(i < 99);
    auto t = stream.Peek();
    if (t && t->type() == token::Type::Symbol) {
      func_node->returns.push_back(t->str());
      stream.Next();
      continue;
    } else {
      break;
    }
  }

  EXPECT_TOKEN_ERR(stream, token::Type::LBrace, "expect {");

  SKIP_TOKEN(stream, token::Type::Enl);

  Result<Block> block_res = Block::parse(stream);
  if (block_res.isErr())
    return Err(block_res.takeErr(), stream.loc(), "parse block error");
  EXPECT_TOKEN(stream, token::Type::RBrace);

  func_node->func_name = func_name;

  func_node->block = std::move(block_res.takeValue());

  guard.finish(func_node->start, func_node->end);
  return Result<Function>::ok(std::move(func_node));
}

Result<If> If::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::If);
  Result<Expr> expr = Expr::parse(stream);
  if (expr.isErr()) return Err(stream.loc(), "parse expr error");

  EXPECT_TOKEN(stream, token::Type::LBrace);

  SKIP_TOKEN(stream, token::Type::Enl);
  Result<Block> block_res = Block::parse(stream);
  if (block_res.isErr())
    return Err(
        block_res.takeErr(), stream.loc(),
        std::string("parse if error at") + stream.state().loc.ToString());

  EXPECT_TOKEN(stream, token::Type::RBrace);

  NodePtr<If> if_node = std::make_unique<If>();

  if_node->expr = std::move(expr.takeValue());
  if_node->block = std::move(block_res.takeValue());

  guard.finish(if_node->start, if_node->end);
  return Result<If>::ok(std::move(if_node));
}

}  // namespace ast
}  // namespace mygo
