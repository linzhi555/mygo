#include <gtest/gtest.h>

#include "ast.h"

TEST(mygo_struct, parse) {
  std::string src = R"(type struct {
    age int
    name string
    weight float
  } Student

  )";

  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  std::cout << stream.debug() << std::endl;

  // EXPECT_FALSE(true) << stream.debug();

  auto root_res = mygo::ast::Root::parse(stream);

  if (root_res.isErr()) {
    std::cout << root_res.takeErr().toString() << std::endl;
  }
  EXPECT_FALSE(root_res.isErr());

  std::cout << root_res.takeValue()->debug() << std::endl;
}
