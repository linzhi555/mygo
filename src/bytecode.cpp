#include "bytecode.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace mygo {

void ByteCodeVM::Init() {
  heap_ = (uint8_t*)calloc(MAX_HEAP, sizeof(uint8_t));
  stack_ = (uint8_t*)calloc(MAX_STACK, sizeof(uint8_t));
}

ByteCodeVM::~ByteCodeVM() {
  if (!heap_) {
    free(heap_);
  }

  if (!stack_) {
    free(stack_);
  }
}

void ByteCodeVM::Run(int ticks) {
  for (int i = 0; i < ticks; i++) {
    if (pc >= program_->instructions.size()) break;
    Instruction& ins = program_->instructions.at(pc + i);
    std::cout << "run" << (uint16_t)ins.op << std::endl;
  }
}

}  // namespace mygo
