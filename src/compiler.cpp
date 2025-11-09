#include "compiler.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <string>

namespace mygo {

std::string VarInfo::debug() {
  std::string res;
  res += name;
  res += std::to_string(type_id);
  return res;
}

std::string TypeInfo::debug() {
  std::string res;
  res += std::to_string(type_id);
  res += " ";
  res += name;
  res += " ";
  res += std::to_string(size);
  if (is_struct) {
    res += " is_struct:yes";
    res += " fields:";
    for (Field field : fields) {
      res += "[";
      res += std::to_string(field.first);
      res += " ";
      res += field.second;
      res += "]";
    }

  } else {
    res += " is_struct:no";
  }

  res += "\n";
  return res;
}

std::string FuncInfo::debug() {
  std::string res;
  res += this->name;

  for (const Instruction& i : instructions) {
    res += "\n";
    res += i.debug();
  }

  return res;
}

void CompilingFunc::addMoveInsts(int dst, int from, int length) {
  for (int i = 0; i < length; i++) {
    instructions.push_back(
        {Op::Move, base(STACK_TOP, dst + i), base(STACK_TOP, from + i), 0});
  }
}

FuncInfo CompilingFunc::FinishCompiling(CompilingFunc&& cp) {
  FuncInfo fi;
  fi.name = std::move(cp.name);
  fi.instructions = std::move(cp.instructions);

  spdlog::info("new function stack size {}", cp.cur_stack_top);

  return fi;
};

TypeTable::TypeTable() { initBasicInfos(); }

void TypeTable::initBasicInfos() {
  TypeInfo temp;
  temp.is_struct = false;
  temp.alias_type = -1;

  temp.type_id = 10;
  temp.size = 1;
  temp.name = "bool";
  insert(temp);

  temp.type_id = 20;
  temp.size = 4;
  temp.name = "int";
  insert(temp);

  temp.type_id = 30;
  temp.size = 4;
  temp.name = "float32";
  insert(temp);

  temp.type_id = 40;
  temp.size = 16;
  temp.name = "string";
  insert(temp);
}

void TypeTable::insert(TypeInfo info) {
  type_table_.insert({info.type_id, std::move(info)});
}

TypeId TypeTable::max_type_id() {
  TypeId max = -1;
  for (auto& [id, _] : type_table_) {
    if (id > max) {
      max = id;
    }
  }
  return max;
}

TypeInfo* TypeTable::findByName(std::string_view name) {
  for (auto& [_, type] : type_table_) {
    if (type.name == name) {
      return &type;
    }
  }
  return nullptr;
}

TypeInfo* TypeTable::findById(TypeId id) {
  if (type_table_.end() == type_table_.find(id)) {
    return nullptr;
  }

  return &type_table_.at(id);
}

std::string TypeTable::debug() {
  std::string res;
  for (auto& [_, t] : type_table_) {
    res += t.debug();
    res += "\n";
  }

  return res;
}

std::string VarTable::debug() {
  std::string res;
  for (VarInfo& var : vars_) {
    res += var.debug();
    res += "\n";
  }

  return res;
}

void VarTable::insert(VarInfo var) { vars_.push_back(var); }

std::string FuncTable::debug() {
  std::string res;
  for (FuncInfo& f : func_infos_) {
    res += f.debug();
    res += "\n";
  }

  return res;
}
void FuncTable::insert(FuncInfo&& f) { func_infos_.emplace_back(std::move(f)); }

void Compiler::compile(const ast::Root& ast) {
  compile_types(ast);
  compile_global(ast);
  compile_funcs(ast);
  link();
}

void Compiler::link() {
  for (const FuncInfo& f : func_table_.funcInfos()) {
    for (const Instruction& i : f.instructions) {
      program_.instructions.push_back(i);
    }
  }
}

int Compiler::getExprSize(const ast::Expr& expr) { return 4; }

void Compiler::compile_global(const ast::Root& ast) {
  for (const std::unique_ptr<ast::Node>& node : ast.nodes()) {
    if (node->type() == ast::Type::Declaration) {
      VarInfo var;
      ast::Declaration* decl = static_cast<ast::Declaration*>(node.get());
      var.name = decl->var_name;
      if (decl->var_type != nullptr) {
        const TypeInfo* tf = type_table_.findByName(decl->var_type->name());
        assert(tf);

        var.type_id = tf->type_id;

      } else {
        assert("temporaily declcation must have type");
      }

      global_table_.insert(var);
    }
  }
}

void Compiler::compile_types(const ast::Root& ast) {
  for (const std::unique_ptr<ast::Node>& node : ast.nodes()) {
    if (node->type() == ast::Type::Typedef) {
      ast::Typedef* tf = static_cast<ast::Typedef*>(node.get());
      TypeInfo type_info;

      type_info.type_id = type_table_.max_type_id() + 10;
      type_info.name = tf->new_name_;
      if (tf->old_type_->is_struct()) {
        type_info.is_struct = true;
        const ast::Struct& stct = tf->old_type_->asStruct();

        type_info.size = 0;
        for (auto [fileld_name, type_str] : stct.fields_) {
          TypeInfo* field_type_info = type_table_.findByName(type_str);
          type_info.size += field_type_info->size;
          assert(field_type_info != nullptr);
          type_info.fields.push_back({field_type_info->type_id, fileld_name});
        }
      }

      type_table_.insert(type_info);
    }
  }
}

void Compiler::compile_funcs(const ast::Root& ast) {
  for (const std::unique_ptr<ast::Node>& node : ast.nodes()) {
    if (node->type() == ast::Type::Function) {
      compile_func(*static_cast<ast::Function*>(node.get()));
    }
  }
}

void Compiler::compile_func(const ast::Function& func) {
  CompilingFunc f;
  f.name = func.func_name;

  setCurrentFunc(&f);

  compile_block(*func.block.get());

  func_table_.insert(CompilingFunc::FinishCompiling(std::move(f)));
}

void Compiler::compile_statement(const ast::Node* stmt) {
  switch (stmt->type()) {
    case ast::Type::Declaration:
      compile_declaration(*static_cast<const ast::Declaration*>(stmt));
      break;
    case ast::Type::Assignment:
      compile_assignment(*static_cast<const ast::Assignment*>(stmt));
      break;
    case ast::Type::If:
      compile_if(*static_cast<const ast::If*>(stmt));
      break;
    case ast::Type::Expr:
      compile_expr(*static_cast<const ast::Expr*>(stmt));
      break;
    case ast::Type::For:
      compile_for(*static_cast<const ast::For*>(stmt));
      break;
    default:
      break;
  }
}

void Compiler::compile_declaration(const ast::Declaration& decl) {
  CompilingFunc* cp = currentFunc();
  assert(cp);

  int var_size = 0;
  if (auto* s = type_table_.findByName(decl.var_type->name())) {
    var_size = s->size;
  } else {
    Error err = "type is not defined: " + decl.var_type->name();
    errors().emplace_back(err);
    spdlog::error(err);
    return;
  }

  VarInfo new_var;
  new_var.name = decl.var_name;
  new_var.size = var_size;
  new_var.address = cp->cur_stack_top;

  cp->var_address_map.emplace(new_var.name, new_var);
  cp->cur_stack_top += var_size;
}

void Compiler::compile_assignment(const ast::Assignment& asmt) {
  CompilingFunc* cp = currentFunc();
  assert(cp);

  const VarInfo& var_info = cp->var_address_map[asmt.var_name];
  if (cp->var_address_map.end() != cp->var_address_map.find(asmt.var_name)) {
    spdlog::info("set var {} of size {} in memory {}", asmt.var_name,
                 var_info.size, var_info.address);
  } else {
    Error err = "var is not defined: " + asmt.var_name;
    errors().emplace_back(err);
    spdlog::error(err);
    return;
  }

  int old_stack_top = cp->cur_stack_top;
  int length = var_info.size;

  compile_expr(*asmt.expr.get());
  cp->addMoveInsts(var_info.address, old_stack_top, length);
}

void Compiler::compile_expr(const ast::Expr& expr) {
  CompilingFunc* cp = currentFunc();
  switch (expr.etype_) {
    case ast::ExprType::FUNCALL:
      compile_funcall(expr);
      break;

    case ast::ExprType::TOKEN:
      compile_expr_literature(expr);
      break;

    case ast::ExprType::OPS:
      compile_expr_ops(expr);
      break;

    default:
      errors().emplace_back("bare expr is not used");
      break;
  }
}

void Compiler::compile_expr_literature(const ast::Expr& expr) {
  CompilingFunc* cp = currentFunc();
  if (expr.v.type_ == token::Type::Int) {
    cp->instructions.push_back(
        {Op::Set32, base(STACK_TOP, cp->cur_stack_top), i32u64(expr.v.i()), 0});
  } else if (expr.v.type_ == token::Type::Float) {
    cp->instructions.push_back(
        {Op::Set32, base(STACK_TOP, cp->cur_stack_top), f32u64(expr.v.f()), 0});
  }
}

void Compiler::compile_expr_ops(const ast::Expr& expr) {
  CompilingFunc* cp = currentFunc();

  int origin_stack_top = cp->cur_stack_top;

  // -(expr) condition like -1 , -2.0 -(1*a)
  if (expr.ops.size() == 1 && expr.exprs.size() == 1 &&
      expr.ops[0] == token::Type::Sub) {
    compile_expr(*expr.exprs[0].get());

    cp->instructions.push_back({Op::MulI32D, base(STACK_TOP, origin_stack_top),
                                i32u64(-1), base(STACK_TOP, origin_stack_top)}

    );

    goto finish;
  }

  compile_expr(*expr.exprs[0].get());
  cp->cur_stack_top += getExprSize(*expr.exprs[0]);

  for (int i = 0; i < expr.ops.size(); i++) {
    token::Type t = expr.ops[i];

    compile_expr(*expr.exprs[i + 1].get());
    switch (t) {
      case token::Type::Plus:
        cp->instructions.push_back({Op::AddI32,
                                    base(STACK_TOP, origin_stack_top),
                                    base(STACK_TOP, cp->cur_stack_top),
                                    base(STACK_TOP, origin_stack_top)});
        break;
      case token::Type::Sub:
        cp->instructions.push_back({Op::SubI32,
                                    base(STACK_TOP, origin_stack_top),
                                    base(STACK_TOP, cp->cur_stack_top),
                                    base(STACK_TOP, origin_stack_top)});

        break;

      case token::Type::Star:
        cp->instructions.push_back({Op::MulI32,
                                    base(STACK_TOP, origin_stack_top),
                                    base(STACK_TOP, cp->cur_stack_top),
                                    base(STACK_TOP, origin_stack_top)});

        break;

      case token::Type::Slash:
        cp->instructions.push_back({Op::DivI32,
                                    base(STACK_TOP, origin_stack_top),
                                    base(STACK_TOP, cp->cur_stack_top),
                                    base(STACK_TOP, origin_stack_top)});

        break;

      default:
        break;
    }
  }

finish:

  cp->cur_stack_top = origin_stack_top;
}

void Compiler::compile_funcall(const ast::Expr& expr) {
  int i = 0;
  std::string func_name;
  for (const ast::NodePtr<ast::Expr>& e : expr.exprs) {
    if (i == 0) {
      func_name = e->v.str();
    } else {
      compile_expr(*e.get());
    }
    i++;
  }

  compile_call(func_name);
  spdlog::info("call func_name {}", func_name);
}

void Compiler::compile_call(std::string_view func_name) {
  CompilingFunc* cp = currentFunc();
  if (func_name == "print") {
    cp->instructions.push_back(
        {Op::Call, mygo::SC_PRINT_I32, base(STACK_TOP, cp->cur_stack_top), 0});
  }
}

void Compiler::compile_if(const ast::If& iff) {
  CompilingFunc* cp = currentFunc();

  Address label = alloc_new_label();
  cp->instructions.push_back({
      Op::JumpEqI32,
      0,
      0,
      label,
  });

  for (const ast::If::Branch& b : iff.branches_) {
    compile_block(*b.second.get());
  }

  if (iff.tail_else_) {
    compile_block(*iff.tail_else_->get());
  }

  cp->instructions.push_back({
      Op::Label,
      label,
      0,
      0,
  });
}

void Compiler::compile_for(const ast::For& forr) {
  compile_block(*forr.block_.get());
}

void Compiler::compile_block(const ast::Block& blk) {
  for (const std::unique_ptr<ast::Node>& node : blk.nodes_) {
    compile_statement(node.get());
  }
}

std::string Compiler::debug() {
  std::string res;

  res += "------TYPES------\n";

  res += type_table_.debug();

  res += "------GLOBALS------\n";
  res += global_table_.debug();

  res += "------FUNCINFOS------\n";
  res += func_table_.debug();

  if (hasError()) {
    for (const Error& err : errors()) {
      res += err;
      res += "\n";
    }
  }

  return res;
}

}  // namespace mygo
