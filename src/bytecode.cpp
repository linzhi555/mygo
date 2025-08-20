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
      case Op::Call:
        if (ins.arg0 == SC_PRINT_U32) {
          printf("%d\n", *(uint32_t*)(baseOffset(ins.arg1)));
        } else if (ins.arg0 == SC_PRINT_U8) {
          printf("%d\n", *(uint8_t*)(baseOffset(ins.arg1)));
        } else if (ins.arg0 == SC_PRINT_STR) {
          printf("%s\n", (char*)(baseOffset(ins.arg1)));
        }

        pc_++;
        break;

      case Op::Set8: {
        *(uint8_t*)baseOffset(ins.arg0) = (uint8_t)ins.arg1;
        pc_++;
        break;
      }

      case Op::Set32: {
        *(uint32_t*)baseOffset(ins.arg0) = (uint32_t)ins.arg1;
        pc_++;
        break;
      }

      case Op::Set64: {
        *(uint64_t*)baseOffset(ins.arg0) = ins.arg1;
        pc_++;
        break;
      }
      case Op::SetSP: {
        sp_ = ins.arg0;
        pc_++;
        break;
      }

      case Op::AddI32: {
        *(uint32_t*)baseOffset(ins.arg0) =
            *(uint32_t*)baseOffset(ins.arg1) + *(uint32_t*)baseOffset(ins.arg2);
        pc_++;
        break;
      }

      case Op::AddI8: {
        *(uint8_t*)baseOffset(ins.arg0) =
            *(uint8_t*)baseOffset(ins.arg1) + *(uint8_t*)baseOffset(ins.arg2);
        pc_++;
        break;
      }

      case Op::SavePC: {
        *(uint64_t*)baseOffset(ins.arg0) = pc_ + 1;
        pc_++;
        break;
      }

      case Op::JumpGtI8: {
        uint8_t a = *(uint8_t*)baseOffset(ins.arg0);
        uint8_t b = *(uint8_t*)baseOffset(ins.arg1);

        if (a > b) {
          pc_ = *(uint64_t*)baseOffset(ins.arg2);
        } else {
          pc_++;
        }
        break;
      }

      default:
        printf("ERROR op is %d \n", (int)ins.op);
        assert("not implement op for this op" && false);
        break;
    }
  }
}

}  // namespace mygo
