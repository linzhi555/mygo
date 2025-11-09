#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "ast.h"
#include "compiler.h"

TEST(Compiler, compile) {
  std::string src = R"(
func main(){

    var c int = 0
    c = -1 + 2 + 3
    print(b)
}
  )";



  std::string src1 = R"(

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

func main(){
    c = -1 + 2 + 3
    print(b)
}

func add(){
    var b float32 =  a + 1
    var c int = 0
    b = 22.3
    c = -9
    if true {
        c = 12 + 11
    }

    c = 3


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

  std::unique_ptr<mygo::Program> program =
      std::make_unique<mygo::Program>(compiler.getResult());

  spdlog::info(compiler.debug());
  std::cout << program->debug() << std::endl;

  mygo::ByteCodeVM vm(std::move(program));

  spdlog::info("run start");
  vm.Run(UINT64_MAX);
  spdlog::info("run end");

  vm.debugStack();
}
