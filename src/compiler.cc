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
