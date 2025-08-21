#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"
TEST(ByteVM, run) {
  auto program = std::make_unique<mygo::Program>();

  // condition variable
  program->instructions.push_back({mygo::Op::Set8, 0, 0, 0});
  program->instructions.push_back({mygo::Op::Set8, 1, 5, 0});
  program->instructions.push_back({mygo::Op::Set8, 2, 1, 0});
  program->instructions.push_back({mygo::Op::Set8, 3, 0, 0});

  program->instructions.push_back({mygo::Op::Set32, 4, mygo::f32u64(3.0), 0});

  program->instructions.push_back({mygo::Op::Set64, 8, 0, 0});

  program->instructions.push_back({mygo::Op::SetSP, 16, 0, 0});

  program->instructions.push_back({mygo::Op::SavePC, 8, 0, 0});
  program->instructions.push_back({mygo::Op::AddI8, 3, 2, 3});
  program->instructions.push_back({mygo::Op::AddF32D, 4, mygo::f32u64(3.0), 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_I8, 0, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_I8, 1, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_I8, 2, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_I8, 3, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_F32, 4, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_STR, 100, 0});
  program->instructions.push_back({mygo::Op::JumpGtI8, 1, 3, 8});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "helloworld");
  vm.Run(100);

  vm.DebugStack();
}
