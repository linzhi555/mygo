#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"
TEST(ByteVM, run) {
  auto program = std::make_unique<mygo::Program>();
  program->instructions.push_back({mygo::Op::Push8, 16, 0, 0});
  program->instructions.push_back({mygo::Op::Push8, 250, 0, 0});
  program->instructions.push_back({mygo::Op::Push8, 3, 0, 0});
  program->instructions.push_back({mygo::Op::Push8, 5, 0, 0});
  program->instructions.push_back({mygo::Op::AddI8I, 3, 2, 3});
  program->instructions.push_back({mygo::Op::AddI8I, 1, 1, 0});

  program->instructions.push_back({mygo::Op::Push32, 564687, 0, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_U8, 0, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_U8, 1, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_U8, 2, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_U8, 3, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_U32, 4, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_STR, 100, 0});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "helloworld");
  vm.Run(100);

  vm.DebugStack();
}
