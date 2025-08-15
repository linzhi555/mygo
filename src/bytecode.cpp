#include "bytecode.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace mygo {

void ByteCodeVM::Init() {
  rom_start_ = 0;
  stack_start_ = rom_start_ + program_->rom.size();
  heap_start_ = stack_start_ + MAX_STACK;

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
    Instruction& ins = program_->instructions.at(pc);

    switch (ins.op) {
      case Op::Push:
        stack_[sp] = ins.arg0;
        sp++;
        break;

      case Op::Call:
        if (ins.arg0 == SC_PRINT_I32) {
          printf("%d\n", stack_[ins.arg1]);
        } else if (ins.arg0 == SC_PRINT_STR) {
          printf("%s\n", (char*)&stack_[ins.arg1]);
        }

        break;
      default:
        printf("ERROR op is %d \n", (int)ins.op);
        assert("not implement op for this op" && false);
    }

    pc++;
  }
}

}  // namespace mygo
