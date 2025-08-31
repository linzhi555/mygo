#pragma once

#include "ast.h"
#include "bytecode.h"
#include "unordered_map"
namespace mygo {

struct Var {
  std::string name;
  int size;
};

struct FuncInfo {
  std::vector<Var> locals;
  std::vector<Instruction> instructions;
  FuncInfo();
  ~FuncInfo();
};

struct TypeInfo {
  int type_id;
  int size = 0;
  std::string name = "";
  bool is_struct = false;
  int alias_type = -1;
  std::vector<int> subs;

  bool is_alias_type() { return alias_type >= 0; }
};

class TypeTable {
  std::unordered_map<int, TypeInfo> type_table_;

 public:
  TypeTable();
};

class Compiler {
  Program program_;
  std::vector<TypeInfo> type_infos_;
  std::vector<Var> global_infos_;
  std::vector<FuncInfo> func_infos_;

  void compile_global();
  void compile_types();
  void compile_funcs();
  void link();

 public:
  void compile(const ast::Root& ast);
};

}  // namespace mygo
