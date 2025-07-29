#include "server.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <uv.h>

#include "channel.h"
#include "task.h"

namespace mygo {

const int MAX_THREAD = 1;

class Loop {
public:
  using Chan = Channel<std::unique_ptr<Task>>;
  Loop(std::string_view id) : id_(id) {
    pending_tasks_ = std::make_unique<Chan>();
    uv_loop_ = uv_default_loop();
  }
  Loop(Loop &&moved) {
    pending_tasks_ = std::move(moved.pending_tasks_);
    moved.pending_tasks_ = nullptr;
    id_ = moved.id_;
    finished_ = false;
    uv_loop_ = moved.uv_loop_;
    moved.uv_loop_ = nullptr;
  }

  ~Loop() { uv_loop_close(uv_loop_); }

  void PostTask(std::unique_ptr<Task> t) { pending_tasks_->Push(std::move(t)); }

  void PostTask(std::function<void(Loop *)> func) {
    pending_tasks_->Push(std::unique_ptr<Task>(new RunOnce(func)));
  }

  void PostTimerTask(bool repeat, std::function<void(Loop *)> func,
                     Duration d) {
    pending_tasks_->Push(std::unique_ptr<Task>(new TimerTask(repeat, func, d)));
  }

  std::string id() { return id_; }
  void run() {
    std::cout << id_ << " started" << std::endl;
    uv_timer_t t;
    uv_timer_init(uv_loop_, &t);
    uv_timer_start(
        &t, [](uv_timer_t *) { std::cout << "timer timeout" << std::endl; },
        100, 100);

    while (pending_tasks_->Size() > 0) {
      uv_run(uv_loop_, UV_RUN_ONCE);
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
  uv_loop_t *uv_loop_ = nullptr;
};

int Server::Run() {
  running_ = true;

  std::vector<Loop> loops;
  std::vector<std::thread> threads;

  for (int i = 0; i < MAX_THREAD; ++i) {
    loops.emplace_back(std::string("Loop" + std::to_string(i)));
  }

  int i = 0;
  for (auto &loop : loops) {
    std::string id = loop.id();
    loop.PostTask([i, id](Loop *this_loop) {
      std::this_thread::sleep_for(Duration(1000));
      std::cout << id << " " << i << std::endl;
      this_loop->PostTimerTask(
          true,
          [i](Loop *) {
            std::cout << "its a callback after" << i << std::endl;
          },
          Duration((i + 1) * 1000));
    });
    i++;

    threads.emplace_back([](Loop *loop) { loop->run(); }, &loop);
  }

  for (std::thread &thread : threads) {
    thread.join();
  }

  running_ = false;
  return 0;
}

int Server::Stop() {
  running_ = false;
  return 0;
}

} // namespace mygo
