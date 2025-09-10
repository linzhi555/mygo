#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "ast.h"
#include "compiler.h"

TEST(Compiler, compile) {
  std::string src = R"(

var a = 11

func main(){
    var b =  a + 1
    print(b)
}

)";

  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));
  auto root_res = mygo::ast::Root::parse(stream);
  EXPECT_FALSE(root_res.isErr()) << root_res.takeErr().toString();

  if (root_res.isErr()) return;

  std::unique_ptr<mygo::ast::Root> root = root_res.takeValue();
  spdlog::info(root->debug());

  mygo::Compiler compiler;
  compiler.compile(*root.get());

  spdlog::info(compiler.debug());
}
