#pragma once
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
namespace mygo {

enum SYSCALL : uint32_t {
  SC_PRINT_U8 = 1000,
  SC_PRINT_U32,
  SC_PRINT_STR,
};

enum class Op : uint32_t {
  Set8,
  Set32,
  Set64,
  SetSP,

  // set 0x0000000 as base address
  BaseZero,

  // set stack top address as base address
  BaseStack,

  AddI8,
  AddI32,
  AddI64,
  AddF32,
  AddF64,

  AddI8D,
  AddI32D,
  AddI64D,
  AddF32D,
  AddF64D,

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

  // inline void* heapOffset(uint64_t offset) {
  //   return transAddress(heap_start_ + offset);
  // }

  // inline void* stackOffset(uint64_t offset) {
  //   return transAddress(stack_start_ + offset);
  // }

  // inline void* romOffset(uint64_t offset) {
  //   return transAddress(rom_start_ + offset);
  // }

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
