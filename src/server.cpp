#include "server.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace mygo {

const int MAX_THREAD = 3;

class Loop {
 public:
  Loop(std::string_view id) : id_(id) {};
  void run() {
    while (!finished_) {
      std::cout << id_ << " is running" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

 private:
  std::string id_;
  bool finished_;
};

int Server::Run() {
  running_ = true;

  std::vector<Loop> loops;
  std::vector<std::thread> threads;

  for (int i = 0; i < MAX_THREAD; ++i) {
    loops.emplace_back(std::string("Loop" + std::to_string(i)));
  }

  for (auto& loop : loops) {
    threads.emplace_back([](Loop* loop) { loop->run(); }, &loop);
  }

  for (std::thread& thread : threads) {
    thread.join();
  }
  running_ = false;
  return 0;
}

int Server::Stop() {
  running_ = false;
  return 0;
}

}  // namespace mygo
