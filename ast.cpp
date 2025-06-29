#include "ast.h"

#include <memory>
#include <string>
#include <utility>

#include "logging.h"

namespace mygo {
namespace ast {

Result<Funcall> Funcall::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Funcall>();

  Result<Expr> f = Expr::parse(stream);
  if (f.isErr()) return Result<Funcall>::err("expect err");
  res->func = std::move(f.takeValue());

  EXPECT_TOKEN(stream, token::Type::LParent);

  while (true) {
    auto expr = Expr::parse(stream);
    if (expr.isOk()) {
      res->arguments.emplace_back(expr.takeValue());
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
  return Result<Funcall>::ok(std::move(res));
};

Result<Block> Block::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto res = std::make_unique<Block>();
  for (int i = 0; i < 10000; i++) {
    std::cout << i << std::endl;
    SKIP_TOKEN(stream, token::Type::Enl);
    {
      Result<Declaration> decl = Declaration::parse(stream);
      if (decl.isOk()) {
        res->nodes_.push_back(decl.takeValue());
        continue;
      }
    }

    {
      Result<Funcall> fcall = Funcall::parse(stream);
      if (fcall.isOk()) {
        res->nodes_.push_back(fcall.takeValue());
        continue;
      }
    }

    {
      Result<If> if_node = If::parse(stream);
      if (if_node.isOk()) {
        res->nodes_.push_back(if_node.takeValue());
        continue;
      }
    }

    {
      Result<Function> func_node = Function::parse(stream);
      if (func_node.isOk()) {
        res->nodes_.push_back(func_node.takeValue());
        continue;
      }
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
  if (e.isErr())
    return Err(std::string("parsing expr error" + e.err_stack.at(0).msg));
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
  if (temp.isErr()) return Result<Expr>::err("parse expr error");
  NodePtr<Expr> expr0 = temp.takeValue();

  auto t = stream.Peek()->type();
  if (t != token::Type::Greater && t != token::Type::Less &&
      t != token::Type::Equal) {
    guard.finish(expr0->start, expr0->end);
    return Result<Expr>::ok(std::move(expr0));
  }

  stream.Next();

  auto multi = std::make_unique<Expr>();

  auto expr1_res = Expr::parse_with_plus(stream);
  if (expr1_res.isErr()) return Result<Expr>::err("parse expr error");

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
    return Result<Expr>::err("expr error error");
  }
  auto t = stream.Peek()->type();
  if (t != token::Type::Plus && t != token::Type::Sub) {
    guard.finish(expr0_res.value->start, expr0_res.value->end);
    return expr0_res;
  }

  auto multi = std::make_unique<Expr>();

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
        return Result<Expr>::err("expect error");
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
    return Result<Expr>::err("parse atomic expr");
  }

  auto t = stream.Peek()->type();
  if (t != token::Type::Star && t != token::Type::Slash) {
    guard.finish(expr0_res.value->start, expr0_res.value->end);
    return expr0_res;
  }

  auto multi = std::make_unique<Expr>();

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
        return Result<Expr>::err("parse atomic error");
      }
    } else {
      break;
    }
  }

  guard.finish(multi->start, multi->end);
  return Result<Expr>::ok(std::move(multi));
}

Result<Expr> Expr::parse_atomic(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  auto v = stream.Peek();
  if (!v) {
    return Result<Expr>::err("parse atomic error");
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
  return Result<Expr>::err("parse atomic error");
};

Result<Function> Function::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::Func);

  NodePtr<Function> func_node = std::make_unique<Function>();
  std::string func_name;
  if (!(stream.Peek()->type() == token::Type::Symbol))
    Result<Expr>::err("expect symbol");
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

  EXPECT_TOKEN_ERR(stream, token::Type::LBrace, "expect {");

  SKIP_TOKEN(stream, token::Type::Enl);

  Result<Block> block = Block::parse(stream);
  if (block.isErr()) return Result<Function>::err("parse function error");
  EXPECT_TOKEN(stream, token::Type::RBrace);

  func_node->func_name = func_name;

  func_node->block = std::move(block.takeValue());

  guard.finish(func_node->start, func_node->end);
  return Result<Function>::ok(std::move(func_node));
}

Result<If> If::parse(TokenStream& stream) {
  TokenStream::StateGuard guard(stream);

  EXPECT_TOKEN(stream, token::Type::If);
  EXPECT_TOKEN(stream, token::Type::LParent);
  Result<Expr> expr = Expr::parse(stream);
  if (expr.isErr()) return Err("parse if error");
  EXPECT_TOKEN(stream, token::Type::RParent);

  EXPECT_TOKEN(stream, token::Type::LBrace);

  SKIP_TOKEN(stream, token::Type::Enl);
  Result<Block> block = Block::parse(stream);
  if (block.isErr()) return Err("parse block error");
  EXPECT_TOKEN(stream, token::Type::RBrace);

  NodePtr<If> if_node = std::make_unique<If>();

  if_node->expr = std::move(expr.takeValue());
  if_node->block = std::move(block.takeValue());

  guard.finish(if_node->start, if_node->end);
  return Result<If>::ok(std::move(if_node));
}

}  // namespace ast
}  // namespace mygo
