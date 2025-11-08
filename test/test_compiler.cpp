#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "ast.h"
#include "compiler.h"

TEST(Compiler, compile) {
  std::string src = R"(

package main

var a float32 = 11
var times int = 1000

var test_name string = "this is a compiler test"

type int myint


type struct{
    name string
    aget int
    weight float32
}Student


func add(){
    var b float32 =  a + 1
    var c int = 0
    b = 22.3
    c = 12
    print(b)
}

func main(){
    var b int =  a + 1
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
