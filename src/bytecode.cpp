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

      // clang-format off
#define CASE1(OPNAME, OP, INS_TYPE, C_TYPE)                                \
  case Op::OPNAME##INS_TYPE: {                                             \
    *(C_TYPE*)baseOffset(ins.arg0) =                                       \
        *(C_TYPE*)baseOffset(ins.arg1) OP * (C_TYPE*)baseOffset(ins.arg2); \
    pc_++;                                                                 \
    break;                                                                 \
  }

#define OP1(INS_TYPE, C_TYPE) CASE1(Add, +, INS_TYPE, C_TYPE)
    BinarInstList(OP1)
#undef OP1

#define OP1(INS_TYPE, C_TYPE) CASE1(Sub, -, INS_TYPE, C_TYPE)
    BinarInstList(OP1)
#undef OP1

#define OP1(INS_TYPE, C_TYPE) CASE1(Mul, *, INS_TYPE, C_TYPE)
    BinarInstList(OP1)
#undef OP1

#define OP1(INS_TYPE, C_TYPE) CASE1(Div, /, INS_TYPE, C_TYPE)
    BinarInstList(OP1)
#undef OP1

#undef CASE1

#define CASE2(OPNAME, OP, INS_TYPE, C_TYPE)                      \
  case Op::OPNAME##INS_TYPE##D: {                                \
    *(C_TYPE*)baseOffset(ins.arg0) OP## = *(C_TYPE*)(&ins.arg1); \
    pc_++;                                                       \
    break;                                                       \
  }

#define OP2(INS_TYPE, C_TYPE) CASE2(Add, +, INS_TYPE, C_TYPE)
    BinarInstList(OP2)
#undef OP2

#define OP2(INS_TYPE, C_TYPE) CASE2(Sub, -, INS_TYPE, C_TYPE)
    BinarInstList(OP2)
#undef OP2

#define OP2(INS_TYPE, C_TYPE) CASE2(Mul, *, INS_TYPE, C_TYPE)
    BinarInstList(OP2)
#undef OP2

#define OP2(INS_TYPE, C_TYPE) CASE2(Div, /, INS_TYPE, C_TYPE)
    BinarInstList(OP2)
#undef OP2

#undef CASE2

#define CASE3(OPNAME, OP, INS_TYPE, C_TYPE)    \
  case Op::Jump##OPNAME##INS_TYPE: {           \
    C_TYPE a = *(C_TYPE*)baseOffset(ins.arg0); \
    C_TYPE b = *(C_TYPE*)baseOffset(ins.arg1); \
                                               \
    if (a OP b) {                              \
      pc_ = *(uint64_t*)baseOffset(ins.arg2);  \
    } else {                                   \
      pc_++;                                   \
    }                                          \
    break;                                     \
  }

#define OP3(INS_TYPE, C_TYPE) CASE3(Gt, >, INS_TYPE, C_TYPE)
    BinarInstList(OP3)
#undef OP3

#define OP3(INS_TYPE, C_TYPE) CASE3(Eq, ==, INS_TYPE, C_TYPE)
    BinarInstList(OP3)
#undef OP3

#define OP3(INS_TYPE, C_TYPE) CASE3(GtEq, >=, INS_TYPE, C_TYPE)
    BinarInstList(OP3)
#undef OP3

#undef CASE3

#define CASE4(TO_TYPE, TO_C_TYPE, INS_TYPE, C_TYPE)         \
  case Op::To##TO_TYPE##INS_TYPE: {                         \
    *(TO_C_TYPE*) baseOffset(ins.arg0) =                    \
            (TO_C_TYPE) (*(C_TYPE*) baseOffset(ins.arg1));  \
    pc_++;                                                  \
    break;                                                  \
  }

#define OP4(INS_TYPE, C_TYPE) CASE4(I32, uint32_t, INS_TYPE, C_TYPE)
    BinarInstList(OP4)
#undef OP4

#define OP4(INS_TYPE, C_TYPE) CASE4(F32, float, INS_TYPE, C_TYPE)
    BinarInstList(OP4)
#undef OP4


#undef CASE4



      case Op::SavePC: {
        *(uint64_t*)baseOffset(ins.arg0) = pc_ + 1;
        pc_++;
        break;
      }

        // clang-format on

      case Op::Call:

        if (ins.arg0 == SC_PRINT_I8) {
          printf("%d\n", *(uint8_t*)(baseOffset(ins.arg1)));
        }

        else if (ins.arg0 == SC_PRINT_I32) {
          printf("%d\n", *(uint32_t*)(baseOffset(ins.arg1)));
        }

        else if (ins.arg0 == SC_PRINT_I64) {
          printf("%ld\n", *(uint64_t*)(baseOffset(ins.arg1)));
        }

        else if (ins.arg0 == SC_PRINT_F32) {
          printf("%f\n", *(float*)(baseOffset(ins.arg1)));
        }

        else if (ins.arg0 == SC_PRINT_F64) {
          printf("%f\n", *(double*)(baseOffset(ins.arg1)));
        }

        else if (ins.arg0 == SC_PRINT_STR) {
          printf("%s\n", (char*)(baseOffset(ins.arg1)));
        }

        pc_++;
        break;

      default:
        printf("ERROR op is %d \n", (int)ins.op);
        assert("not implement op for this op" && false);
        break;
    }
  }
}  // namespace mygo

}  // namespace mygo
