#include <gtest/gtest.h>

#include <memory>

#include "bytecode.h"
TEST(ByteVM, run) {
  auto program = std::make_unique<mygo::Program>();
  program->instructions.push_back({mygo::Op::Push, 10, 0, 0});
  program->instructions.push_back({mygo::Op::Push, 20, 0, 0});
  program->instructions.push_back({mygo::Op::Call, 0, 0, 0});

  mygo::ByteCodeVM vm(std::move(program));
  vm.Run(100);
}
