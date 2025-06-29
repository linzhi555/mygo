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

  auto root_res = mygo::ast::Root::parse(stream);
  assert(root_res.isOk());
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(WARNING) << root->start << root->end << stream.state().loc;

  assert(root->end.line == 7);
  assert(root->end.coloum == 1);
  LOG(WARNING) << "test succeed" << std::endl;
}

TEST(2) {
  std::string src = R"(

var a = 11.1001231
func t1 (arg1 float , arg2 int){
      var a = 12;
      print ("do func t1, a is ",a , " args is " , arg1 , " " , arg2)
}

t1(6.66, 88); 
  
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  assert(root_res.isOk());
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(INFO) << root->debug();

  LOG(WARNING) << root->nodes_.size() << std::endl;
  LOG(WARNING) << stream.parse_error << std::endl;

  assert(root->nodes_.size() == 3);

  LOG(WARNING) << root->start << root->end << stream.state().loc << std::endl;

  mygo::VM vm;
  vm.run(root);
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(INFO);

  RUN_TEST;
}
