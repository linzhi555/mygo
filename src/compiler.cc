#include "compiler.h"

#include <spdlog/spdlog.h>

namespace mygo {

std::string VarInfo::debug() {
  std::string res;
  res += name;
  return res;
}

std::string TypeInfo::debug() {
  std::string res;
  res += type_id;
  res += name;
  res += "\n";
  return res;
}

std::string FuncInfo::debug() {
  std::string res;
  res += this->name;
  return res;
}

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
  temp.name = "float";
  insert(temp);

  temp.type_id = 40;
  temp.size = 16;
  temp.name = "string";
  insert(temp);
}

void TypeTable::insert(TypeInfo info) {
  type_table_.insert({info.type_id, std::move(info)});
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
void FuncTable::insert(FuncInfo f) { func_infos_.push_back(f); }

void Compiler::compile(const ast::Root& ast) {
  compile_types(ast);
  compile_global(ast);
  compile_funcs(ast);
  link();
}

void Compiler::compile_global(const ast::Root& ast) {
  for (const std::unique_ptr<ast::Node>& node : ast.nodes()) {
    if (node->type() == ast::Type::Declaration) {
      VarInfo var;
      var.name = static_cast<ast::Declaration*>(node.get())->var_name;
      global_table_.insert(var);
    }
  }
}

void Compiler::compile_types(const ast::Root& ast) {
  for (const std::unique_ptr<ast::Node>& node : ast.nodes()) {
    if (node->type() == ast::Type::Typedef) {
      ast::Typedef* tf = static_cast<ast::Typedef*>(node.get());
      TypeInfo type_info;
      type_info.name = tf->name_;
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
  FuncInfo f;
  f.name = func.func_name;
  func_table_.insert(f);
}

void Compiler::link() {}
std::string Compiler::debug() {
  std::string res;

  res += "------TYPES------\n";

  res += type_table_.debug();

  res += "------GLOBALS------\n";
  res += global_table_.debug();

  res += "------FUNCINFOS------\n";
  res += func_table_.debug();

  return res;
}

}  // namespace mygo
