#include "loop.h"
#include "uv.h"
namespace mygo {

Task::State TimerTask::run(Loop *loop) {

  if (!inited_) {
    uv_timer_init(loop->uv_loop_, &this->uv_timer_);
    uv_timer_.data = &this->timeout_;

    auto timer_cb = [](uv_timer_t *timer) {
      bool *flag_ptr = (bool *)timer->data;
      *flag_ptr = true;
      std::cout << "timer event emit" << std::endl;
    };

    std::cout << this->duration_ << std::endl;

    uint64_t d = (int)this->duration_ / MiliSec;

    uv_timer_start(&uv_timer_, timer_cb, d, d);

    inited_ = true;
    return State::NotFinish;
  }

  if (finished_)
    return State::Finish;
  if (timeout_) {
    func_(loop);
    if (!repeat_) {
      finished_ = true;
    } else {
      timeout_ = false;
    }
  }
  return State::NotFinish;
}

void Loop::run() {
  std::cout << id_ << " started" << std::endl;

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

} // namespace mygo
