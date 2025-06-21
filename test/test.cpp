#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "ast.h"
#include "logging.h"

void test_decl() {
  std::string src = "var a = 11";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));
  auto root = mygo::ast::Declaration::parse(stream);
  assert(root.has_value());
  assert(root.value()->end.coloum - root.value()->start.coloum ==
         (int)src.length());
  LOG(WARNING) << "test succeed" << std::endl;
}

void test_func() {
  std::string src = R"(func t1 (){
      var a = 11.1001231
      print ("run func",a)
  }
  
  
func ssfafdsfwe)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));
  auto root = mygo::ast::Root::parse(stream);
  assert(root.has_value());

  LOG(WARNING) << root.value()->start << root.value()->end
               << stream.state().loc;

  // assert(root.value()->end.line == 5);
  // assert(root.value()->end.coloum == 1);
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(INFO);
  test_decl();
  test_func();
}
