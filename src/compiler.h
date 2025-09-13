#pragma once

#include "ast.h"
#include "unordered_map"
#include "vm.h"
namespace mygo {

using TypeId = int32_t;

struct VarInfo {
  using Type = std::string;
  using Scope = std::string;
  std::string name;
  TypeId type_id;
  int size;
  Scope scope;

  const Scope global_scope = "::";

  std::string debug();

  static bool inScope(const Scope& a, const Scope& b) {
    return b.starts_with(a);
  }

  static Scope appendScope(const Scope& a, const Scope& b) {
    return a + b + "::";
  }
};

struct FuncInfo {
  std::string name;
  std::vector<VarInfo> locals;
  std::vector<Instruction> instructions;

  std::string debug();
  FuncInfo() = default;
  ~FuncInfo() = default;
};

struct TypeInfo {
  TypeId type_id;
  int32_t size = 0;
  std::string name = "";
  bool is_struct = false;
  TypeId alias_type = -1;
  std::vector<TypeId> subs;

  TypeInfo(TypeInfo&&) = default;
  TypeInfo(TypeInfo&) = default;
  TypeInfo() = default;
  std::string debug();
  bool isAliasType() { return alias_type >= 0; }
};

class TypeTable {
 public:
  TypeTable();
  std::string debug();
  void insert(TypeInfo info);

 private:
  void initBasicInfos();

  std::unordered_map<TypeId, TypeInfo> type_table_;
};

class VarTable {
 public:
  void insert(VarInfo);
  std::string debug();

 private:
  std::vector<VarInfo> vars_;
};

class FuncTable {
 public:
  void insert(FuncInfo);
  std::string debug();

 private:
  std::vector<FuncInfo> func_infos_;
};

class Compiler {
  Program program_;
  TypeTable type_table_;
  VarTable global_table_;
  FuncTable func_table_;

  void compile_func(const ast::Function& func);

  void compile_global(const ast::Root& ast);
  void compile_types(const ast::Root& ast);
  void compile_funcs(const ast::Root& ast);
  void link();

 public:
  std::string debug();
  Compiler() = default;
  void compile(const ast::Root& ast);
};

}  // namespace mygo
