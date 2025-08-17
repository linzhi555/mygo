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
  base_addr_ = stack_start_;

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
    if (pc_ >= program_->instructions.size()) break;
    Instruction& ins = program_->instructions.at(pc_);

    switch (ins.op) {
      case Op::Push8: {
        *(uint8_t*)(&stack_[sp_]) = (uint8_t)ins.arg0;
        sp_ += 1;
        break;
      }

      case Op::Push32: {
        *(uint32_t*)(&stack_[sp_]) = ins.arg0;
        sp_ += 4;
        break;
      }

      case Op::Call:
        if (ins.arg0 == SC_PRINT_U32) {
          printf("%d\n", *(uint32_t*)(baseOffset(ins.arg1)));
        } else if (ins.arg0 == SC_PRINT_U8) {
          printf("%d\n", *(uint8_t*)(baseOffset(ins.arg1)));
        } else if (ins.arg0 == SC_PRINT_STR) {
          printf("%s\n", (char*)(baseOffset(ins.arg1)));
        }

        break;
      default:
        printf("ERROR op is %d \n", (int)ins.op);
        assert("not implement op for this op" && false);
        break;
    }

    pc_++;
  }
}

}  // namespace mygo
