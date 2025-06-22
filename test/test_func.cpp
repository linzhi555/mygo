#include "ast.h"
#include "logging.h"
#include "test.h"

INIT_TEST;

std::vector<void (*)()> test_fs;
TEST(1) {
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

  assert(root.value()->end.line == 7);
  assert(root.value()->end.coloum == 1);
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(INFO);

  RUN_TEST;
}
