#include <gtest/gtest.h>

#include "ast.h"
#include "vm.h"

TEST(IfFor, test1) {
  std::string src = R"(
    var a = 11;
    print(a)


for var i = 10, i > 0,  i = i - 1 {
    print(i)
}


)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr()) << root_res.takeErr().toString();

  std::cout << root_res.value_->debug() << std::endl;

  mygo::VM vm;
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
  vm.run(root);
}
