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

void ByteCodeVM::Run(uint64_t ticks) {
  for (uint64_t i = 0; i < ticks && !finished_; i++) {
    if (pc_ >= program_->instructions.size()) break;
    Instruction& ins = program_->instructions.at(pc_);

    switch (ins.op) {
      case Op::Set8: {
        *(uint8_t*)transAddr(ins.arg0) = (uint8_t)ins.arg1;
        pc_++;
        break;
      }

      case Op::Set32: {
        *(uint32_t*)transAddr(ins.arg0) = (uint32_t)ins.arg1;
        pc_++;
        break;
      }

      case Op::Set64: {
        *(uint64_t*)transAddr(ins.arg0) = ins.arg1;
        pc_++;
        break;
      }
      case Op::SetStackBottom: {
        stack_bottom_ = stack_top_ + ins.arg0;
        pc_++;
        break;
      }

      // clang-format off
#define CASE1(OPNAME, OP, INS_TYPE, C_TYPE)                                \
  case Op::OPNAME##INS_TYPE: {                                             \
    *(C_TYPE*)transAddr(ins.arg0) =                                       \
        *(C_TYPE*)transAddr(ins.arg1) OP * (C_TYPE*)transAddr(ins.arg2); \
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
    *(C_TYPE*)transAddr(ins.arg0) OP## = *(C_TYPE*)(&ins.arg1); \
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
    C_TYPE a = *(C_TYPE*)transAddr(ins.arg0); \
    C_TYPE b = *(C_TYPE*)transAddr(ins.arg1); \
                                               \
    if (a OP b) {                              \
      pc_ = *(uint64_t*)transAddr(ins.arg2);  \
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
    *(TO_C_TYPE*) transAddr(ins.arg0) =                    \
            (TO_C_TYPE) (*(C_TYPE*) transAddr(ins.arg1));  \
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
        *(uint64_t*)transAddr(ins.arg0) = pc_ + 1;
        pc_++;
        break;
      }

        // clang-format on

      case Op::Call:

        // do funcall
        if (ins.arg0 < BUILT_IN_START) {
          call_stack_.push_back({.pc = (pc_ + 1),
                                 .stack_top = stack_top_,
                                 .stack_bottom = stack_bottom_,
                                 .ret_res_addr = ins.arg1});
          pc_ = ins.arg0;
          stack_top_ = stack_bottom_;
          break;
        }

        // bultin function
        switch (ins.arg0) {
          case SC_PRINT_I8:
            printf("%d\n", *(uint8_t*)(transAddr(ins.arg1)));
            break;
          case SC_PRINT_I32:

            printf("%d\n", *(uint32_t*)(transAddr(ins.arg1)));
            break;
          case SC_PRINT_I64:
            printf("%ld\n", *(uint64_t*)(transAddr(ins.arg1)));
            break;
          case SC_PRINT_F32:
            printf("%f\n", *(float*)(transAddr(ins.arg1)));
            break;
          case SC_PRINT_F64:
            printf("%f\n", *(double*)(transAddr(ins.arg1)));
            break;
          case SC_PRINT_STR:
            printf("%s\n", (char*)(transAddr(ins.arg1)));
            break;
          default:
            assert("not implement op for this op" && false);
            break;
        }

        pc_++;

        break;

      case Op::Return: {
        if (call_stack_.empty()) {
          finished_ = true;
          printf("process finished \n");
          break;
        }

        uint64_t ret_addr = ins.arg0;
        uint64_t ret_len = ins.arg1;

        std::vector<uint8_t> ret_content;
        for (uint64_t i = 0; i < ret_len; i++) {
          ret_content.push_back(*(uint8_t*)(transAddr(ret_addr + i)));
        }

        CallPoint cp = call_stack_.back();
        call_stack_.pop_back();
        stack_top_ = cp.stack_top;
        stack_bottom_ = cp.stack_bottom;
        base_addr_ = stack_top_;
        pc_ = cp.pc;
        for (size_t i = 0; i < ret_content.size(); i++) {
          *(uint8_t*)(transAddr(cp.ret_res_addr + i)) = ret_content[i];
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
