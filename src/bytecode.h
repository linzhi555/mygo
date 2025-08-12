#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
namespace mygo {
enum class Op : uint16_t {
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
};

struct Program {
  std::string name;
  std::vector<Instruction> instructions;
  std::vector<uint8_t> data_zone;
};

class ByteCodeVM {
 public:
  ByteCodeVM(std::unique_ptr<Program> program)
      : program_(std::move(program)) {};
  void Run(int ticks) {
    for (int i = 0; i < ticks; i++) {
      if (pc >= program_->instructions.size()) break;
      Instruction& ins = program_->instructions.at(pc + i);
      std::cout << "run" << (uint16_t)ins.op << std::endl;
    }
  }

  size_t pc;
  std::unique_ptr<Program> program_;
};

}  // namespace mygo
