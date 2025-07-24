#pragma once
#include <cassert>
namespace mygo {

class Server {
 public:
  Server() = default;
  ~Server() { assert(running_ == false); };

  int Run();
  int Stop();

 private:
  bool running_ = false;
};

}  // namespace mygo
