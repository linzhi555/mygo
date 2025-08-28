#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "bytecode.h"

namespace mygo {

bool need_debug = true;

std::vector<Instruction> gen_cal_pi_codes() {
  std::vector<Instruction> result;
  bool need_debug = false;

  result.push_back({Op::SetStackBottom, 200, 0, 0});

  uint64_t count_loc = 28;
  uint64_t one_loc = 4;
  uint64_t sum_loc = 8;
  uint64_t add_loc = 12;
  uint64_t deno_loc = 16;
  uint64_t four_loc = 20;
  uint64_t pi_loc = 24;
  uint64_t times_loc = 0;
  uint64_t pi_int_loc = 32;
  uint64_t pc_loc = 40;

  result.push_back({Op::BaseStackTop, 0, 0, 0});
  result.push_back({Op::Set32, count_loc, 0, 0});
  result.push_back({Op::Set32, one_loc, mygo::f32u64(1.0), 0});
  result.push_back({Op::Set32, sum_loc, mygo::f32u64(0.0), 0});
  result.push_back({Op::Set32, add_loc, mygo::f32u64(0.0), 0});
  result.push_back({Op::Set32, deno_loc, mygo::f32u64(1.0), 0});
  result.push_back({Op::Set32, four_loc, mygo::f32u64(4.0), 0});
  result.push_back({Op::Set32, pi_loc, mygo::f32u64(0.0), 0});

  result.push_back({Op::Set64, pc_loc, 0, 0});
  result.push_back({Op::SavePC, pc_loc, 0, 0});

  result.push_back({Op::AddI32D, count_loc, 1, 0});
  result.push_back({Op::DivF32, add_loc, one_loc, deno_loc});
  result.push_back({Op::AddF32, sum_loc, sum_loc, add_loc});
  result.push_back({Op::AddF32D, deno_loc, mygo::f32u64(2.0), 0});
  result.push_back({Op::MulF32D, one_loc, mygo::f32u64(-1.0), 0});
  result.push_back({Op::MulF32, pi_loc, four_loc, sum_loc});

  if (need_debug) {
    result.push_back({mygo::Op::Call, mygo::SC_PRINT_I32, count_loc, 0});
    result.push_back({mygo::Op::Call, mygo::SC_PRINT_F32, sum_loc, 0});
    result.push_back({mygo::Op::Call, mygo::SC_PRINT_F32, pi_loc, 0});
  }

  result.push_back({Op::JumpGtI32, times_loc, count_loc, pc_loc});
  result.push_back({Op::ToI32F32, pi_int_loc, pi_loc, 0});
  result.push_back({Op::Call, mygo::SC_PRINT_STR, 100, 0});
  result.push_back({Op::Call, mygo::SC_PRINT_F32, pi_loc, 0});
  result.push_back({Op::Call, mygo::SC_PRINT_I32, pi_int_loc, 0});
  result.push_back({Op::Call, mygo::SC_PRINT_I32, times_loc, 0});
  result.push_back({Op::Return, 0, 0, 0});

  return result;
}

TEST(ByteVM, cal_pi) {
  auto program = std::make_unique<mygo::Program>();

  auto calpi = gen_cal_pi_codes();
  for (auto i : calpi) {
    program->AddInstruction(i);
  }

  uint64_t main_loc = program->instructions.size();

  program->AddInstruction({Op::SetStackBottom, 20, 0, 0});
  program->AddInstruction({Op::BaseStackBottom, 0, 0, 0});
  program->AddInstruction({Op::Set32, 0, 1000, 0});
  program->AddInstruction({Op::Call, 0, 0, 0});

  program->AddInstruction({Op::BaseStackBottom, 0, 0, 0});
  program->AddInstruction({Op::Set32, 0, 200, 0});
  program->AddInstruction({Op::Call, 0, 0, 0});

  mygo::ByteCodeVM vm(std::move(program));
  vm.setPc(main_loc);

  snprintf((char*)vm.transAddress(vm.stack_start() + 100), 20, "pi is");
  vm.Run(UINT64_MAX);
  std::cout << UINT64_MAX << std::endl;

  vm.DebugStack();
}

}  // namespace mygo
