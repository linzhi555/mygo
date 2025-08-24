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
  const uint64_t pc_addr = 40;
  bool need_debug = true;

  program->AddInstruction({mygo::Op::SetSP, 16, 0, 0});

  program->AddInstruction({mygo::Op::Set32, 0, 0, 0});
  program->AddInstruction({mygo::Op::Set32, 4, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, 8, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, 12, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, 16, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, 20, mygo::f32u64(4.0), 0});
  program->AddInstruction({mygo::Op::Set32, 24, mygo::f32u64(0.0), 0});

  program->AddInstruction({mygo::Op::Set32, 28, times, 0});
  program->AddInstruction({mygo::Op::Set64, pc_addr, 0, 0});
  program->AddInstruction({mygo::Op::SavePC, pc_addr, 0, 0});

  program->AddInstruction({mygo::Op::AddI32D, 0, 1, 0});
  program->AddInstruction({mygo::Op::DivF32, 12, 4, 16});
  program->AddInstruction({mygo::Op::AddF32, 8, 8, 12});
  program->AddInstruction({mygo::Op::AddF32D, 16, mygo::f32u64(2.0), 0});
  program->AddInstruction({mygo::Op::MulF32D, 4, mygo::f32u64(-1.0), 0});
  program->AddInstruction({mygo::Op::MulF32, 24, 20, 8});

  if (need_debug) {
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I32, 0, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 4, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 8, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 12, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 16, 0});
    program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 24, 0});
  }
  program->AddInstruction({mygo::Op::JumpGtI32, 28, 0, pc_addr});

  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_STR, 100, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 24, 0});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "pi is");
  vm.Run(times * 20);

  vm.DebugStack();
}
