#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ast.h"
#include "logging.h"
#include "token_stream.h"
#include "vm.h"

// std::string src = R"(var a=1
// print("hello world","123",123)
//)";

int main(int argc, char** argv) {
  // set_debug_level(NOLOG);
  set_debug_level(INFO);
  LOG(INFO) << "args";
  for (int i = 0; i < argc; i++) {
    LOG(INFO) << " " << argv[i];
  }

  if (argc == 2 && std::string(argv[1]) == "--help") {
    std::cout << "mygo v0.1, author: lzq, all right reserved" << std::endl;
    std::cout << "usage: mygo xxx.mgo" << std::endl;
    return 0;
  }

  std::string file;
  if (argc == 2) {
    file = std::string(argv[1]);
  }

  std::ifstream fstream(file);
  if (!fstream.is_open()) {
    LOG(ERROR) << "fail to read file" << std::endl;
    return -1;
  }
  std::ostringstream oss;
  oss << fstream.rdbuf();       // 将文件内容全部读入字符串流
  std::string src = oss.str();  // 返回字符串

  std::vector<uint8_t> src_data(src.begin(), src.end());

  auto stream = mygo::TokenStream(std::move(src_data));
  mygo::VM vm;
  mygo::ast::Result<mygo::ast::Root> root_res = mygo::ast::Root::parse(stream);
  if (root_res.isOk()) {
    mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
    LOG(INFO) << "root ast: " << root->nodes_.size() << root->debug()
              << std::endl;
    vm.run(root);
    // std::cout << root.value()->debug() << std::endl;
  } else {
    std::cout << "parse error:" << root_res.takeErr().toString() << std::endl;
  }
}
