#pragma once

#include "ast.h"
#include "bytecode.h"
#include "unordered_map"
namespace mygo {

using TypeId = int32_t;

struct Var {
  using Type = std::string;
  using Scope = std::string;
  std::string name;
  TypeId type_id;
  int size;
  Scope scope;

  const Scope global_scope = "::";

  static bool inScope(const Scope& a, const Scope& b) {
    return b.starts_with(a);
  }

  static Scope appendScope(const Scope& a, const Scope& b) {
    return a + b + "::";
  }
};

struct FuncInfo {
  std::vector<Var> locals;
  std::vector<Instruction> instructions;
  FuncInfo();
  ~FuncInfo();
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

  bool isAliasType() { return alias_type >= 0; }
};

class TypeTable {
 public:
  TypeTable();

 private:
  void initBasicInfos();

  void insert(TypeInfo info);
  std::unordered_map<TypeId, TypeInfo> type_table_;
};

class Compiler {
  Program program_;
  std::vector<TypeInfo> type_infos_;
  std::vector<Var> global_infos_;
  std::vector<FuncInfo> func_infos_;


  void compile_func(const ast::Function& func);

  void compile_global(const ast::Root& ast);
  void compile_types(const ast::Root& ast);
  void compile_funcs(const ast::Root& ast);
  void link();

 public:
  Compiler() = default;
  void compile(const ast::Root& ast);
};

}  // namespace mygo
