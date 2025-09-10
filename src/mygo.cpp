#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ast.h"
#include "server.h"
#include "token_stream.h"

#include <spdlog/spdlog.h>



int main(int argc, char** argv) {
  spdlog::info("hellowlrd");

  for (int i = 0; i < argc; i++) {
  }

  if (argc == 2 && std::string(argv[1]) == "--help") {
    std::cout << "mygo v0.1, author: lzq, all right reserved" << std::endl;
    std::cout << "usage: mygo xxx.mgo" << std::endl;
    return 0;
  }

  std::string file;
  if (argc == 2) {
    file = std::string(argv[1]);
  } else {
    mygo::Server server;
    server.Run();
    return 0;
  }

  std::ifstream fstream(file);
  if (!fstream.is_open()) {
    spdlog::error("file open fail");
    return -1;
  }
  std::ostringstream oss;
  oss << fstream.rdbuf();       // 将文件内容全部读入字符串流
  std::string src = oss.str();  // 返回字符串

  std::vector<uint8_t> src_data(src.begin(), src.end());

  auto stream = mygo::TokenStream(std::move(src_data));
  mygo::ast::Result<mygo::ast::Root> root_res = mygo::ast::Root::parse(stream);
  if (root_res.isOk()) {
    mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();
    spdlog::info(root->debug());
  } else {
    spdlog::info(root_res.takeErr().toString());
  }
}
