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
  Call,
  Push8,
  Push32,
  Return,
  Jump,
  SetI,
  SetF,
  SetS,
  Add,
  Sub,
  Mul,
  JumpEqual,
  JumpTrue,
};

struct Instruction {
  Op op;
  uint64_t arg0;
  uint64_t arg1;
  uint64_t arg3;
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

  inline void* transAddress(uint64_t address) {
    if (address >= heap_start_) {
      return &heap_[address];
    } else if (address >= stack_start_) {
      return &stack_[address];
    } else {
      return &program_->rom[address];
    }
  }

  inline void* heapOffset(uint64_t offset) {
    return transAddress(heap_start_ + offset);
  }

  inline void* stackOffset(uint64_t offset) {
    return transAddress(stack_start_ + offset);
  }

  inline void* romOffset(uint64_t offset) {
    return transAddress(rom_start_ + offset);
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

 private:
  void Init();
  std::unique_ptr<Program> program_;

  const uint64_t MAX_HEAP = 1000 * 1000 * 1000;
  const uint64_t MAX_STACK = 1000 * 1000 * 8;

  uint64_t stack_start_;
  uint64_t heap_start_;
  uint64_t rom_start_ = 0;

  uint8_t* heap_;
  uint8_t* stack_;
};

}  // namespace mygo
