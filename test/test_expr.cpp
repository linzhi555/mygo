#include <gtest/gtest.h>

#include "ast.h"
#include "logging.h"
#include "vm.h"

TEST(expr, funcall) {
  std::string src = R"(
func acc (a int) int {
    if a < 2 {
        return a
    }
    return a + acc(a-1)
}

var result = acc(5)

print(result)
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(true) << root_res.takeValue()->debug();

  mygo::VM vm;
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
  vm.run(root);
}
