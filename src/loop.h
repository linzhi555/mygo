#include <uv.h>

#include <functional>
#include <iostream>
#include <memory>

#include "channel.h"
#include "common.h"
namespace mygo {
class Loop;
class Task {
 public:
  enum class State {
    Finish,
    NotFinish,
    Error,
  };

  virtual ~Task() {};

  virtual State run(Loop* l) = 0;
};

class RunOnce : public Task {
 public:
  RunOnce(std::function<void(Loop*)> f) { func_ = f; }
  State run(Loop* l) override {
    if (did_run_) return State::Finish;
    func_(l);
    did_run_ = true;
    return State::Finish;
  }
  ~RunOnce() override = default;

 protected:
  bool did_run_ = false;
  std::function<void(Loop*)> func_;
};

class TimerTask : public Task {
 public:
  TimerTask(bool repeat, std::function<void(Loop*)> f, Duration duration)
      : repeat_(repeat), duration_(duration), func_(f) {}

  State run(Loop* loop) override;
  bool timeOut() { return timeout_; }
  ~TimerTask() override = default;

 private:
  bool inited_ = false;
  bool finished_ = false;
  bool repeat_ = false;
  bool timeout_ = false;
  uv_timer_t uv_timer_;
  Duration duration_;
  std::function<void(Loop*)> func_;
};

// TODO: need implement TcpServerTask
class TcpServerTask : public Task {
 public:
  using ConnectId = int;

  virtual void OnConnect(Loop&, ConnectId id) {
    std::cout << "new connction" << id << std::endl;
  }

  virtual void OnClose(Loop&, ConnectId id) {
    std::cout << "connction close" << id << std::endl;
  }

  virtual void OnData(Loop&, ConnectId id, std::vector<uint8_t> data) {
    std::cout << "receive data on connection " << id << " size " << data.size()
              << std::endl;
  }

  State run(Loop* loop) override;
  TcpServerTask(IP ip, Port port) : ip_(ip), port_(port) {};

  uv_loop_t* uv_loop_;

 private:
  bool inited_ = false;
  bool err_ = false;
  IP ip_;
  Port port_;
  struct sockaddr_in addr_;
  uv_tcp_t server_;
  std::vector<ConnectId> connctions_;
};

class Loop {
 public:
  using Chan = Channel<std::unique_ptr<Task>>;
  Loop(std::string_view id) : id_(id) {
    pending_tasks_ = std::make_unique<Chan>();
    uv_loop_ = uv_default_loop();
  }
  Loop(Loop&& moved) {
    pending_tasks_ = std::move(moved.pending_tasks_);
    moved.pending_tasks_ = nullptr;
    id_ = moved.id_;
    finished_ = false;
    uv_loop_ = moved.uv_loop_;
    moved.uv_loop_ = nullptr;
  }

  ~Loop() { uv_loop_close(uv_loop_); }

  void PostTask(std::unique_ptr<Task> t) { pending_tasks_->Push(std::move(t)); }

  void PostTask(std::function<void(Loop*)> func) {
    pending_tasks_->Push(std::unique_ptr<Task>(new RunOnce(func)));
  }

  void PostTimerTask(bool repeat, std::function<void(Loop*)> func, Duration d) {
    pending_tasks_->Push(std::unique_ptr<Task>(new TimerTask(repeat, func, d)));
  }

  std::string id() { return id_; }
  void run();

  uv_loop_t* uv_loop_ = nullptr;

 private:
  std::unique_ptr<Chan> pending_tasks_;
  std::string id_;
  bool finished_ = false;
};

}  // namespace mygo
