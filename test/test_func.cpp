#include <utility>

#include "ast.h"
#include "logging.h"
#include "test.h"
#include "vm.h"

INIT_TEST;

TEST(1) {
  std::string src = R"(func t1 () int{
      var a = 11.1001231
      print ("run func",a)
      return
  }
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  if (root_res.isErr()) {
    LOG(WARNING) << " root res err: " << root_res.takeErr().toString()
                 << std::endl;
  }

  assert(root_res.isOk());
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(WARNING) << root->debug() << stream.state().loc;

  assert(root->end.line == 6);
  assert(root->end.coloum == 1);
  LOG(WARNING) << "test succeed" << std::endl;
}

TEST(2) {
  std::string src = R"(

func t1 (arg1 float , arg2 int){
    if (true) {
        var a =
    }
}

  
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  assert(root_res.isErr());
  LOG(WARNING) << "get error (as we expected) : "
               << root_res.takeErr().toString() << std::endl;

  LOG(WARNING) << "test succeed" << std::endl;
}

TEST(3) {
  std::string src = R"(

var a = 11.1001231
func t1 (arg1 float , arg2 int) int {
      var a = 12;

      print ("do func t1, a is ",a , " args is " , arg1 , " " , arg2)

      return a

      print ("do func t1, a is ",a , " args is " , arg1 , " " , arg2)
}

t1(6.66, 88); 
  
)";
  std::vector<uint8_t> src_data(src.begin(), src.end());
  auto stream = mygo::TokenStream(std::move(src_data));

  auto root_res = mygo::ast::Root::parse(stream);
  assert(root_res.isOk());
  mygo::ast::NodePtr<mygo::ast::Root> root = root_res.takeValue();

  LOG(WARNING) << root->debug();

  LOG(WARNING) << root->nodes_.size() << std::endl;

  assert(root->nodes_.size() == 3);

  LOG(WARNING) << root->start << root->end << stream.state().loc << std::endl;

  mygo::VM vm;
  vm.run(root);
  LOG(WARNING) << "test succeed" << std::endl;
}

int main() {
  set_debug_level(WARNING);

  RUN_TEST;
}
