#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"

TEST(ByteVM, run) {
  auto program = std::make_unique<mygo::Program>();

  // condition variable
  //

  program->AddInstruction({mygo::Op::SetSP, 16, 0, 0});
  program->AddInstruction({mygo::Op::Set8, 0, 0, 0});
  program->AddInstruction({mygo::Op::Set8, 1, 200, 0});
  program->AddInstruction({mygo::Op::Set8, 2, 1, 0});
  program->AddInstruction({mygo::Op::Set8, 3, 0, 0});
  program->AddInstruction({mygo::Op::Set32, 4, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, 8, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, 12, mygo::f32u64(0.0), 0});
  program->AddInstruction({mygo::Op::Set32, 16, mygo::f32u64(1.0), 0});
  program->AddInstruction({mygo::Op::Set32, 20, mygo::f32u64(4.0), 0});
  program->AddInstruction({mygo::Op::Set32, 24, mygo::f32u64(0.0), 0});

  const uint64_t pc_addr = 40;
  program->AddInstruction({mygo::Op::Set64, pc_addr, 0, 0});
  program->AddInstruction({mygo::Op::SavePC, pc_addr, 0, 0});

  program->AddInstruction({mygo::Op::AddI8, 3, 2, 3});
  program->AddInstruction({mygo::Op::DivF32, 12, 4, 16});
  program->AddInstruction({mygo::Op::AddF32, 8, 8, 12});
  program->AddInstruction({mygo::Op::AddF32D, 16, mygo::f32u64(2.0), 0});
  program->AddInstruction({mygo::Op::MulF32D, 4, mygo::f32u64(-1.0), 0});
  program->AddInstruction({mygo::Op::MulF32, 24, 20, 8});

  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I8, 0, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I8, 1, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I8, 2, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_I8, 3, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 4, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 8, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 12, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 16, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_F32, 24, 0});
  program->AddInstruction({mygo::Op::Call, mygo::SC_PRINT_STR, 100, 0});
  program->AddInstruction({mygo::Op::JumpGtI8, 1, 3, pc_addr});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "helloworld");
  vm.Run(10000);

  vm.DebugStack();
}
