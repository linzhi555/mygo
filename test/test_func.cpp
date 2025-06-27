#include <utility>

#include "ast.h"
#include "logging.h"
#include "test.h"
#include "vm.h"

INIT_TEST;

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

TEST(2) {
  std::string src = R"(

var a = 11.1001231
func t1 (v float , b int){
      var a = 12;
      print ("run func",a)
}

t1(); 
  
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));
  auto root = mygo::ast::Root::parse(stream);
  assert(root.has_value());

  LOG(INFO) << root.value()->debug();

  LOG(WARNING) << root.value()->nodes_.size() << std::endl;
  LOG(WARNING) << stream.parse_error << std::endl;

  assert(root.value()->nodes_.size() == 3);

  LOG(WARNING) << root.value()->start << root.value()->end << stream.state().loc
               << std::endl;

  mygo::VM vm;
  vm.run(root.value());
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(INFO);

  RUN_TEST;
}
