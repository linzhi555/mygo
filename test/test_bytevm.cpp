#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"

TEST(ByteVM, cal_pi) {
  auto program = std::make_unique<mygo::Program>();

  // condition variable
  //
  //
  //

  uint64_t times = 10000;
  bool need_debug = true;

  program->AddInstruction({mygo::Op::SetSP, 0, 0, 0});

  uint64_t count_loc = 0;
  uint64_t one_loc = 4;
  uint64_t sum_loc = 8;
  uint64_t add_loc = 12;
  uint64_t deno_loc = 16;
  uint64_t four_loc = 20;
  uint64_t pi_loc = 24;
  uint64_t times_loc = 28;
  uint64_t pi_int_loc = 32;
  uint64_t pc_loc = 40;

  program->AddInstruction({mygo::Op::Set32, count_loc, 0, 0});
  program->AddInstruction({mygo::Op::Set32, one_loc, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, sum_loc, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, add_loc, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, deno_loc, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, four_loc, mygo::f32u64(4.0), 0});
  program->AddInstruction({mygo::Op::Set32, pi_loc, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, times_loc, times, 0});

  program->AddInstruction({mygo::Op::Set64, pc_loc, 0, 0});
  program->AddInstruction({mygo::Op::SavePC, pc_loc, 0, 0});

  program->AddInstruction({mygo::Op::AddI32D, count_loc, 1, 0});
  program->AddInstruction({mygo::Op::DivF32, add_loc, one_loc, deno_loc});
  program->AddInstruction({mygo::Op::AddF32, sum_loc, sum_loc, add_loc});
  program->AddInstruction({mygo::Op::AddF32D, deno_loc, mygo::f32u64(2.0), 0});
  program->AddInstruction({mygo::Op::MulF32D, one_loc, mygo::f32u64(-1.0), 0});
  program->AddInstruction({mygo::Op::MulF32, pi_loc, four_loc, sum_loc});

  if (need_debug) {
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I32, count_loc, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, sum_loc, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, pi_loc, 0});
  }
  program->AddInstruction({mygo::Op::JumpGtI32, times_loc, count_loc, pc_loc});

  program->AddInstruction({mygo::Op::ToI32F32, pi_int_loc, pi_loc, 0});

  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_STR, 100, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, pi_loc, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I32, pi_int_loc, 0});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "pi is");
  vm.Run(times * 20);

  vm.DebugStack();
}
