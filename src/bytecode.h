#pragma once
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
namespace mygo {

enum SYSCALL : uint32_t {
  SC_PRINT_I8 = 1000,
  SC_PRINT_I32,
  SC_PRINT_I64,
  SC_PRINT_F32,
  SC_PRINT_F64,
  SC_PRINT_STR,
};

// clang-format off
#define BinarInstList(OP1,OP2) \
  OP1(I8,uint8_t) \
  OP1(I32,uint32_t)\
  OP1(I64, uint64_t)\
  OP1(F32,float)\
  OP1(F64,double)\
  OP2(I8D,uint8_t) \
  OP2(I32D,uint32_t)\
  OP2(I64D, uint64_t)\
  OP2(F32D,float)\
  OP2(F64D,double)

// clang-format on

enum class Op : uint32_t {
  Set8,
  Set32,
  Set64,
  SetSP,
  // set 0x0000000 as base address
  BaseZero,

  // set stack top address as base address
  BaseStack,

// clang-format off
#define ADD(INS_TYPE, C_TYPE) Add##INS_TYPE ,
#define SUB(INS_TYPE, C_TYPE) Sub##INS_TYPE ,
#define MUL(INS_TYPE, C_TYPE) Mul##INS_TYPE ,
#define DIV(INS_TYPE, C_TYPE) Div##INS_TYPE ,

  BinarInstList(ADD, ADD)
  BinarInstList(SUB, SUB) 
  BinarInstList(MUL, MUL)
  BinarInstList(DIV, DIV)

#undef ADD
#undef SUB
#undef MUL
#undef DIV
  // clang-format on
  //

  SavePC,
  Call,
  Return,

  Jump,
  JumpGtI8,
  JumpGtI32,
  JumpGtI64,

  JumpGtF32,
  JumpGtF64,

  JumpEqI8,
  JumpEqI32,
  JumpEqI64,

};

struct Instruction {
  Op op;
  uint64_t arg0;
  uint64_t arg1;
  uint64_t arg2;
};

struct Program {
  std::string name = "a.myout";

  std::vector<uint8_t> rom;
  std::vector<Instruction> instructions;

  void Save(std::string _) {}
  void Load(std::string _) {}
};

inline uint64_t f32u64(float f) {
  uint64_t res = 0;
  res = *(uint64_t*)(&f);
  return res;
}

class ByteCodeVM {
 public:
  ByteCodeVM(std::unique_ptr<Program> program) : program_(std::move(program)) {
    Init();
  };

  ~ByteCodeVM();

  void Run(int ticks);
  // program counter
  uint64_t pc_ = 0;
  // stacktop
  uint64_t sp_ = 0;

  std::array<uint64_t, 10> registers_{{0}};

  inline void* transAddress(uint64_t absolute) {
    if (absolute >= heap_start_) {
      return &heap_[absolute];
    } else if (absolute >= stack_start_) {
      return &stack_[absolute];
    } else {
      return &program_->rom[absolute];
    }
  }

  inline uint64_t toAbsolute(uint64_t offset) { return base_addr_ + offset; }

  inline void* baseOffset(uint64_t offset) {
    return transAddress(toAbsolute(offset));
  }

  void DebugStack() {
    for (uint64_t i = 0; i < sp_; i++) {
      printf("%3d ", stack_[i]);
    }
    printf("\n");

    for (uint64_t i = 0; i < sp_ && i < 255; i++) {
      printf("%3d ", (int)i);
    }
    printf("\n");
  }

  uint64_t stack_start() { return stack_start_; }

 private:
  void Init();
  std::unique_ptr<Program> program_;

  const uint64_t MAX_HEAP = 1000 * 1000 * 1000;
  const uint64_t MAX_STACK = 1000 * 1000 * 8;

  uint64_t stack_start_;
  uint64_t heap_start_;
  uint64_t rom_start_ = 0;
  uint64_t base_addr_;

  uint8_t* heap_;
  uint8_t* stack_;
};

}  // namespace mygo
