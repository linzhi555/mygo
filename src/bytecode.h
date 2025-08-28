#pragma once
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
namespace mygo {

const uint64_t BUILT_IN_START = UINT64_MAX - 10000;
enum SYSCALL : uint64_t {
  SC_PRINT_I8 = BUILT_IN_START,
  SC_PRINT_I32,
  SC_PRINT_I64,
  SC_PRINT_F32,
  SC_PRINT_F64,
  SC_PRINT_STR,
};

// clang-format off
#define BinarInstList(OP) \
  OP(I8,uint8_t) \
  OP(I32,uint32_t)\
  OP(I64, uint64_t)\
  OP(F32,float)\
  OP(F64,double)

// clang-format on

enum class Op : uint32_t {
  Set8,
  Set32,
  Set64,

  // set the size of the stack
  SetStackBottom,

  // set 0x0000000 as base address
  BaseZero,

  // set stack top address as base address
  BaseStackTop,

  // set the stack bottom addres as the base address
  BaseStackBottom,

// clang-format off
#define ADD(INS_TYPE, C_TYPE) Add##INS_TYPE ,
  BinarInstList(ADD)
#undef ADD

#define SUB(INS_TYPE, C_TYPE) Sub##INS_TYPE ,
  BinarInstList(SUB)
#undef SUB

#define MUL(INS_TYPE, C_TYPE) Mul##INS_TYPE ,
  BinarInstList(MUL)
#undef MUL

#define DIV(INS_TYPE, C_TYPE) Div##INS_TYPE ,
  BinarInstList(DIV)
#undef DIV

#define ADDD(INS_TYPE, C_TYPE) Add##INS_TYPE##D ,
  BinarInstList(ADDD)
#undef ADDD

#define SUBD(INS_TYPE, C_TYPE) Sub##INS_TYPE##D ,
  BinarInstList(SUBD) 
#undef SUBD

#define MULD(INS_TYPE, C_TYPE) Mul##INS_TYPE##D ,
  BinarInstList(MULD)
#undef MULD

#define DIVD(INS_TYPE, C_TYPE) Div##INS_TYPE##D ,
  BinarInstList(DIVD)
#undef DIVD

#define JumpGt(INS_TYPE, C_TYPE) JumpGt##INS_TYPE ,
  BinarInstList(JumpGt)
#undef JumpGt

#define JumpEq(INS_TYPE, C_TYPE) JumpEq##INS_TYPE ,
  BinarInstList(JumpEq)
#undef JumpEq

#define JumpGtEq(INS_TYPE, C_TYPE) JumpGtEq##INS_TYPE ,
  BinarInstList(JumpGtEq)
#undef JumpGtEq

#define ToI32(INS_TYPE, C_TYPE) ToI32##INS_TYPE ,
  BinarInstList(ToI32)
#undef ToI32

#define ToF32(INS_TYPE, C_TYPE) ToF32##INS_TYPE ,
  BinarInstList(ToF32)
#undef ToF32

  // clang-format on
  //

  SavePC,
  Call,
  Return,

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

  void AddInstruction(Instruction insc) { instructions.push_back(insc); }

  void Save(std::string _) {}
  void Load(std::string _) {}
};

inline uint64_t f32u64(float f) {
  uint64_t res = 0;
  res = *(uint64_t*)(&f);
  return res;
}

struct CallPoint {
  uint64_t pc;
  uint64_t stack_top;
  uint64_t stack_bottom;
};

using CallStack = std::vector<CallPoint>;

class ByteCodeVM {
 public:
  ByteCodeVM(std::unique_ptr<Program> program) : program_(std::move(program)) {
    Init();
  };

  ~ByteCodeVM();

  void Run(uint64_t ticks);

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
    const uint64_t line_width = 20;
    for (int j = 0; j < 200; j += line_width) {
      for (uint64_t i = j; i < j + line_width; i++) {
        printf("%3d ", stack_[i]);
      }
      printf("\n");

      for (uint64_t i = j; i < j + line_width; i++) {
        printf("%3d ", (int)i);
      }
      printf("\n");
      printf("\n");
      printf("\n");
    }
  }

  uint64_t stack_start() { return stack_start_; }

  void setPc(uint64_t pc) { pc_ = pc; }

 private:
  void Init();
  std::unique_ptr<Program> program_;

  const uint64_t MAX_HEAP = 1000 * 1000 * 1000;
  const uint64_t MAX_STACK = 1000 * 1000 * 8;

  // program counter
  uint64_t pc_ = 0;
  // stacktop
  uint64_t stack_top_ = 0;
  uint64_t stack_bottom_ = 0;

  uint64_t rom_start_ = 0;
  uint64_t stack_start_;
  uint64_t heap_start_;
  uint64_t base_addr_;

  CallStack call_stack_;

  uint8_t* heap_;
  uint8_t* stack_;
};

}  // namespace mygo
