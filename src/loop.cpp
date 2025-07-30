#include "loop.h"
namespace mygo {
void Loop::run() {
  std::cout << id_ << " started" << std::endl;
  uv_timer_t t;
  uv_timer_init(uv_loop_, &t);
  uv_timer_start(
      &t, [](uv_timer_t *) { std::cout << "timer timeout" << std::endl; }, 100,
      100);

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
