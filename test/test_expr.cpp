#include <gtest/gtest.h>

#include "ast.h"
#include "logging.h"
#include "vm.h"

TEST(expr, calculator) {
  std::string src = R"(
var a = -14 + 3 * 3 - 3
var b = 3.3 * 3.0 / 2 - 4.4
var c = 3-1
print(a)
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  // EXPECT_FALSE(true) << stream.debug();

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr());

  mygo::VM vm;
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
  vm.run(root);

  EXPECT_EQ(vm.global().Get("a")->As<int>(), -8);
  EXPECT_EQ(vm.global().Get("c")->As<int>(), 2);
  EXPECT_GT(vm.global().Get("b")->As<float>(), 0.54);
  EXPECT_LT(vm.global().Get("b")->As<float>(), 0.56);
}

TEST(expr, funcall) {
  std::string src = R"(
func acc (a int) int {
    if a < 2 {
        return a
    }
    return a + acc(a - 1)
}

var result = acc(5)

print(result)
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr());

  mygo::VM vm;
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
  vm.run(root);
  ASSERT_EQ(vm.exit_, mygo::VM::ExitNormal);
}
