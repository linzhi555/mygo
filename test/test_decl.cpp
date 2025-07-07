#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "ast.h"
#include "logging.h"
#include "test/test.h"

INIT_TEST;

TEST(1) {
  std::string src = "var a = 11";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));
  auto decl_res = mygo::ast::Declaration::parse(stream);
  assert(decl_res.isOk());
  mygo::ast::NodePtr<mygo::ast::Declaration> decl = decl_res.takeValue();

  assert(decl->end.coloum - decl->start.coloum == (int)src.length());
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(INFO);
  RUN_ALL_TEST;
}
