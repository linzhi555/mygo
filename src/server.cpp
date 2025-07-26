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

  void PostTask(std::function<void(Loop*)> func) {
    pending_tasks_->Push(std::unique_ptr<Task>(new RunOnce(func)));
  }

  void PostTaskDelay(std::function<void(Loop*)> func, Duration d) {
    pending_tasks_->Push(std::unique_ptr<Task>(new RunOnceDelay(func, d)));
  }

  std::string id() { return id_; }
  void run() {
    std::cout << id_ << " started" << std::endl;
    while (pending_tasks_->Size() > 0) {
      std::unique_ptr<Task> task = pending_tasks_->Pop();
      Task::State state = task->run(this);
      if (state == Task::State::NotFinish) {
        pending_tasks_->Push(std::move(task));
      }
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

  int i = 0;
  for (auto& loop : loops) {
    std::string id = loop.id();
    loop.PostTask([i, id](Loop* this_loop) {
      std::this_thread::sleep_for(Duration(1000));
      std::cout << id << " " << i << std::endl;
      this_loop->PostTaskDelay(
          [i](Loop*) { std::cout << "its a callback after" << i << std::endl; },
          Duration((i + 1) * 1000));
    });
    i++;

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
