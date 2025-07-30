#pragma once
#include "channel.h"
#include "task.h"
#include <memory>
#include <uv.h>
namespace mygo {
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
  void run();

private:
  std::unique_ptr<Chan> pending_tasks_;
  std::string id_;
  bool finished_ = false;
  uv_loop_t *uv_loop_ = nullptr;
};

} // namespace mygo
