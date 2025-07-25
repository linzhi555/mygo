#include "server.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "channel.h"
#include "task.h"

namespace mygo {

const int MAX_THREAD = 3;

class Loop {
 public:
  using Chan = Channel<std::unique_ptr<Task>>;
  Loop(std::string_view id) : id_(id) {
    pending_tasks_ = std::make_unique<Chan>();
  }
  Loop(Loop&& moved) {
    pending_tasks_ = std::move(moved.pending_tasks_);
    moved.pending_tasks_ = nullptr;
    id_ = moved.id_;
    finished_ = false;
  }

  void PostTask(std::unique_ptr<Task> t) { pending_tasks_->Push(std::move(t)); }

  std::string id() { return id_; }
  void run() {
    std::cout << id_ << " started" << std::endl;
    while (pending_tasks_->Size() > 0) {
      std::unique_ptr<Task> task = pending_tasks_->Pop();
      task->run();
    }
    finished_ = true;
  }

 private:
  std::unique_ptr<Chan> pending_tasks_;
  std::string id_;
  bool finished_ = false;
};

int Server::Run() {
  running_ = true;

  std::vector<Loop> loops;
  std::vector<std::thread> threads;

  for (int i = 0; i < MAX_THREAD; ++i) {
    loops.emplace_back(std::string("Loop" + std::to_string(i)));
  }

  for (auto& loop : loops) {
    for (int i = 0; i < 100; i++) {
      std::string id = loop.id();
      loop.PostTask(std::unique_ptr<Task>(new RunOnce([i, id]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << id << " " << i << std::endl;
      })));
    }

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
