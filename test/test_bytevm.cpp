#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"
TEST(ByteVM, run) {
  auto program = std::make_unique<mygo::Program>();
  program->instructions.push_back({mygo::Op::Push, 10, 0, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_I32, 0, 0});
  program->instructions.push_back({mygo::Op::Call, mygo::SC_PRINT_STR, 1, 0});

  mygo::ByteCodeVM vm(std::move(program));

  snprintf((char*)vm.stackOffset(1), 100, "helloworld");
  vm.Run(100);
}
