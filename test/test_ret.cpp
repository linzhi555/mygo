#include <gtest/gtest.h>

#include "ast.h"
#include "logging.h"
#include "vm.h"

TEST(ret, vm_ret) {
  std::string src = R"(
  return 11 * 11 - 1
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  ASSERT_TRUE(root_res.isOk());
  auto root = root_res.takeValue();
  mygo::VM vm;
  vm.run(root);

  ASSERT_EQ(vm.exit_, mygo::VM::ExitNormal);
  ASSERT_TRUE(vm.ret_.As<int>() == 120);
}

TEST(test, Accumulate) {
  std::string src = R"(
func acc (a int) int {
    if a < 2 {
        return a
    }
    return a + acc(a-1)
}

result = acc(5)

print(result)
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  if (root_res.isErr()) {
    LOG(WARNING) << root_res.err_.toString();
  }

  ASSERT_TRUE(root_res.isOk());

  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(WARNING) << root->debug();

  LOG(WARNING) << root->nodes_.size() << std::endl;

  LOG(WARNING) << root->start << root->end << stream.state().loc << std::endl;

  mygo::VM vm;
  vm.run(root);
  LOG(WARNING) << "test succeed" << std::endl;
}
