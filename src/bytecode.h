#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace mygo {
enum class Op : uint32_t {
  Call,
  Push,
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

  std::vector<uint8_t> data_zone;
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
  uint64_t pc = 0;
  // stacktop
  uint64_t sp = 0;

  std::array<uint64_t, 10> registers_{{0}};

 private:
  void Init();
  std::unique_ptr<Program> program_;

  const uint64_t MAX_HEAP = 1000 * 1000 * 1000;
  const uint64_t MAX_STACK = 1000 * 1000 * 8;

  uint8_t* heap_;
  uint8_t* stack_;
};

}  // namespace mygo
