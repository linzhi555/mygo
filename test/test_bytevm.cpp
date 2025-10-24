#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "vm.h"

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

  result.push_back({Op::Set32, base(STACK_TOP, count_loc), 0, 0});
  result.push_back({Op::Set32, base(STACK_TOP, one_loc), mygo::f32u64(1.0), 0});
  result.push_back({Op::Set32, base(STACK_TOP, sum_loc), mygo::f32u64(0.0), 0});
  result.push_back({Op::Set32, base(STACK_TOP, add_loc), mygo::f32u64(0.0), 0});
  result.push_back(
      {Op::Set32, base(STACK_TOP, deno_loc), mygo::f32u64(1.0), 0});
  result.push_back(
      {Op::Set32, base(STACK_TOP, four_loc), mygo::f32u64(4.0), 0});
  result.push_back({Op::Set32, base(STACK_TOP, pi_loc), mygo::f32u64(0.0), 0});

  result.push_back({Op::Set64, base(STACK_TOP, pc_loc), 0, 0});
  result.push_back({Op::SavePC, base(STACK_TOP, pc_loc), 0, 0});

  result.push_back({Op::AddI32D, base(STACK_TOP, count_loc), 1, 0});
  result.push_back({Op::DivF32, base(STACK_TOP, add_loc),
                    base(STACK_TOP, one_loc), base(STACK_TOP, deno_loc)});
  result.push_back({Op::AddF32, base(STACK_TOP, sum_loc),
                    base(STACK_TOP, sum_loc), base(STACK_TOP, add_loc)});
  result.push_back(
      {Op::AddF32D, base(STACK_TOP, deno_loc), mygo::f32u64(2.0), 0});
  result.push_back(
      {Op::MulF32D, base(STACK_TOP, one_loc), mygo::f32u64(-1.0), 0});
  result.push_back({Op::MulF32, base(STACK_TOP, pi_loc),
                    base(STACK_TOP, four_loc), base(STACK_TOP, sum_loc)});

  if (need_debug) {
    result.push_back(
        {mygo::Op::Call, mygo::SC_PRINT_I32, base(STACK_TOP, count_loc), 0});
    result.push_back(
        {mygo::Op::Call, mygo::SC_PRINT_F32, base(STACK_TOP, sum_loc), 0});
    result.push_back(
        {mygo::Op::Call, mygo::SC_PRINT_F32, base(STACK_TOP, pi_loc), 0});
  }

  result.push_back({Op::JumpGtI32, base(STACK_TOP, times_loc),
                    base(STACK_TOP, count_loc), base(STACK_TOP, pc_loc)});
  result.push_back(
      {Op::ToI32F32, base(STACK_TOP, pi_int_loc), base(STACK_TOP, pi_loc), 0});
  // result.push_back({Op::Call, mygo::SC_PRINT_STR, 100, 0});
  // result.push_back({Op::Call, mygo::SC_PRINT_F32, pi_loc, 0});
  // result.push_back({Op::Call, mygo::SC_PRINT_I32, pi_int_loc, 0});
  // result.push_back({Op::Call, mygo::SC_PRINT_I32, times_loc, 0});
  result.push_back({Op::Return, base(STACK_TOP, pi_loc), 4, 0});

  return result;
}

TEST(ByteVM, cal_pi) {
  auto program = std::make_unique<mygo::Program>();

  auto calpi = gen_cal_pi_codes();
  for (auto i : calpi) {
    program->AddInstruction(i);
  }

  uint64_t main_loc = program->instructions.size();
  uint64_t ret_loc = 4;

  program->AddInstruction(
      {Op::Set32, base(STACK_TOP, ret_loc), f32u64(4.0), 0});
  program->AddInstruction(
      {Op::Call, mygo::SC_PRINT_F32, base(STACK_TOP, ret_loc), 0});

  program->AddInstruction({Op::SetStackBottom, 20, 0, 0});
  program->AddInstruction({Op::Set32, base(STACK_TAIL, 0), 200, 0});
  program->AddInstruction({Op::Call, 0, base(STACK_TOP, ret_loc), 0});

  program->AddInstruction(
      {Op::Call, mygo::SC_PRINT_F32, base(STACK_TOP, ret_loc), 0});

  program->AddInstruction({Op::Set32, base(STACK_TAIL, 0), 10000, 0});
  program->AddInstruction({Op::Call, 0, base(STACK_TOP, ret_loc), 0});

  program->AddInstruction(
      {Op::Call, mygo::SC_PRINT_F32, base(STACK_TOP, ret_loc), 0});

  program->AddInstruction({Op::Return, 0, 0, 0});

  mygo::ByteCodeVM vm(std::move(program));
  vm.setPc(main_loc);

  snprintf((char*)vm.transAddr(base(STACK_TOP, 100)), 20, "pi is");
  vm.Run(UINT64_MAX);
  std::cout << UINT64_MAX << std::endl;

  vm.debugStack();
}

}  // namespace mygo
