#include <gtest/gtest.h>

#include "ast.h"
#include "logging.h"
#include "vm.h"

TEST(test, helloworld) { EXPECT_EQ(1, 2); }

TEST(test, Accumulate) {
  std::string src = R"(
func acc (a int) int {
    if a < 2 {
        
    }
    return a + acc(a-1)
}

print(acc(5))
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  if (root_res.isErr()) {
    LOG(WARNING) << root_res.err_.toString();
  }

  EXPECT_TRUE(root_res.isOk());

  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(WARNING) << root->debug();

  LOG(WARNING) << root->nodes_.size() << std::endl;

  assert(root->nodes_.size() == 3);

  LOG(WARNING) << root->start << root->end << stream.state().loc << std::endl;

  mygo::VM vm;
  vm.run(root);
  LOG(WARNING) << "test succeed" << std::endl;
}
