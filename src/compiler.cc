#include "compiler.h"

#include "iostream"
namespace mygo {

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

void Compiler::compile(const ast::Root& ast) {
  compile_global(ast);
  compile_types(ast);
  compile_funcs(ast);
  link();
}

void Compiler::compile_global(const ast::Root& ast) {}
void Compiler::compile_types(const ast::Root& ast) {}
void Compiler::compile_funcs(const ast::Root& ast) {
  for (std::unique_ptr<ast::Node>& node : ast.block_->nodes_) {
    if (node->Type() == ast::Type::Function) {
      compile_func(*static_cast<ast::Function*>(node.get()));
    }
  }
}

void Compiler::compile_func(const ast::Function& func) {}

}  // namespace mygo
