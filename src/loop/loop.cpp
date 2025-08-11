#include "loop.h"

#include <uv.h>

#include <cstdlib>

namespace mygo {

Task::State TimerTask::run(Loop* loop) {
  if (state_.isS0()) {
    uv_timer_init(loop->uv_loop_, &this->uv_timer_);
    uv_timer_.data = &this->timeout_;

    auto timer_cb = [](uv_timer_t* timer) {
      bool* flag_ptr = (bool*)timer->data;
      *flag_ptr = true;
      std::cout << "timer event emit" << std::endl;
    };

    std::cout << this->duration_ << std::endl;

    uint64_t d = (int)this->duration_ / MiliSec;

    uv_timer_start(&uv_timer_, timer_cb, d, d);

    state_.next();
    return state_;
  }

  if (state_.isFinished()) return state_;
  if (timeout_) {
    func_(loop);
    if (!repeat_) {
      state_.setFinished();
    } else {
      timeout_ = false;
    }
  }
  return state_;
}

void Loop::run() {
  std::cout << id_ << " started" << std::endl;

  while (true) {
    std::cout << "uv run once" << std::endl;

    std::vector<std::unique_ptr<Task>> not_finished;
    while (pending_tasks_->Size() > 0) {
      std::unique_ptr<Task> task = pending_tasks_->Pop();
      Task::State state = task->run(this);
      if (state.isNotFinished()) {
        not_finished.push_back(std::move(task));
      }
    }

    for (std::unique_ptr<Task>& task : not_finished) {
      pending_tasks_->Push(std::move(task));
    }

    uv_run(uv_loop_, UV_RUN_ONCE);
  }
  finished_ = true;
}

}  // namespace mygo
