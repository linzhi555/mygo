#pragma once

#include <string_view>

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
  int address;
  Scope scope;

  const Scope global_scope = "::";

  VarInfo(const VarInfo&) = default;
  VarInfo() = default;

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
  FuncInfo(FuncInfo&&) = default;
  ~FuncInfo() = default;
};

struct CompilingFunc {
  std::string name;
  int cur_stack_top = 0;

  using Scope = std::unordered_map<std::string, VarInfo>;
  Scope var_address_map;
  std::vector<Instruction> instructions;

  std::string debug();

  void addMoveInsts(int dst, int from, int length);

  static FuncInfo FinishCompiling(CompilingFunc&& cp);
  CompilingFunc() = default;
  ~CompilingFunc() = default;
};

struct TypeInfo {
  using Field = std::pair<TypeId, std::string>;
  TypeId type_id;
  int32_t size = 0;
  std::string name = "";
  bool is_struct = false;
  TypeId alias_type = -1;
  std::vector<Field> fields;

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
  TypeId max_type_id();

  // return TypeInfo* according the TypeName, nullptr if not found
  TypeInfo* findByName(std::string_view name);

  TypeInfo* findById(TypeId id);

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
  void insert(FuncInfo&&);
  std::string debug();

  const std::vector<FuncInfo>& funcInfos() { return func_infos_; }

 private:
  std::vector<FuncInfo> func_infos_;
};

class Compiler {
  Program program_;
  TypeTable type_table_;
  VarTable global_table_;
  FuncTable func_table_;
  CompilingFunc* current_func_ = nullptr;

  using Error = std::string;
  std::vector<Error> errors_;

  int getValSize(std::string_view val_type);
  void compile_global(const ast::Root& ast);
  void compile_types(const ast::Root& ast);
  void compile_funcs(const ast::Root& ast);

  void compile_func(const ast::Function& func);
  void compile_statement(const ast::Node* stmt);
  void compile_declaration(const ast::Declaration& decl);
  void compile_assignment(const ast::Assignment& asgm);

  void compile_expr(const ast::Expr& expr);
  void compile_literature(const ast::Expr& expr);
  void compile_funcall(const ast::Expr& expr);
  void compile_call(std::string_view func_name);
  void compile_if(const ast::If& iff);
  void compile_for(const ast::For& forr);
  void compile_block(const ast::Block& blk);

  void link();

 public:
  CompilingFunc* currentFunc() { return current_func_; }
  void setCurrentFunc(CompilingFunc* f) { current_func_ = f; }
  Compiler() = default;
  bool hasError() { return !errors_.empty(); };
  std::vector<Error>& errors() { return errors_; };
  std::string debug();
  const Program& getResult() { return program_; }
  void compile(const ast::Root& ast);
};

}  // namespace mygo
