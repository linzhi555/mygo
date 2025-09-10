#include <gtest/gtest.h>

#include "ast.h"
#include "logging.h"
#include "vm.h"

TEST(IfAst, error) {
  std::string src = R"(
func judge (a int {
    if  a > 89 {
        print(a," your score is A")
        return
    }else if a > 79{
        print(a," your score is B")
        return
    } else if a > 69 {
        print(a," your score is C")
        return
    } else if a > 59 {
        print(a," your score is D")
        return
    } else {
        print(a," your score is F")
    }

    print(a," score finish----")
}

judge(91)
judge(30)
judge(60)
judge(81)

)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr()) << root_res.takeErr().toString();
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  std::cout << root->debug() << std::endl;
}


TEST(IfAst, score) {
  std::string src = R"(
func judge (a int) {
    if  a > 89 {
        print(a," your score is A")
        return
    }else if a > 79{
        print(a," your score is B")
        return
    } else if a > 69 {
        print(a," your score is C")
        return
    } else if a > 59 {
        print(a," your score is D")
        return
    } else {
        print(a," your score is F")
    }

    print(a," score finish----")
}

judge(91)
judge(30)
judge(60)
judge(81)


)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr()) << root_res.takeErr().toString();
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  std::cout << root->debug() << std::endl;
}
