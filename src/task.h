#pragma once
#include <cstdint>
#include <functional>
#include <iostream>

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

  virtual ~Task(){};

  virtual State run(Loop *l) = 0;
};

class RunOnce : public Task {
public:
  RunOnce(std::function<void(Loop *)> f) { func_ = f; }
  State run(Loop *l) override {
    if (did_run_)
      return State::Finish;
    func_(l);
    did_run_ = true;
    return State::Finish;
  }
  ~RunOnce() override = default;

protected:
  bool did_run_ = false;
  std::function<void(Loop *)> func_;
};

class TimerTask : public Task {
public:
  TimerTask(bool repeat, std::function<void(Loop *)> f,
            std::chrono::milliseconds delay)
      : repeat_(repeat), init_time_(std::chrono::steady_clock::now()),
        delay_(delay), func_(f) {}

  State run(Loop *l) override {
    if (finished_)
      return State::Finish;
    if (timeOut()) {
      func_(l);

      if (!repeat_) {
        finished_ = true;
        return State::Finish;
      } else {
        init_time_ = std::chrono::steady_clock::now();
        return State::NotFinish;
      }
    }
    return State::NotFinish;
  }

  bool timeOut() {
    Time now = std::chrono::steady_clock::now();
    Duration elapsed = std::chrono::duration_cast<Duration>(now - init_time_);
    if (elapsed > delay_) {
      return true;
    }

    return false;
  }
  ~TimerTask() override = default;

private:
  bool finished_ = false;
  bool repeat_;
  Time init_time_;
  Duration delay_;
  std::function<void(Loop *)> func_;
};

// TODO: need implement TcpServerTask
class TcpServerTask : public Task {

public:
  using ConnectId = int;

  virtual void OnConnect(Loop &, ConnectId id) {
    std::cout << "new connction" << id << std::endl;
  }

  virtual void OnClose(Loop &, ConnectId id) {
    std::cout << "connction close" << id << std::endl;
  }

  virtual void OnData(Loop &, ConnectId id, std::vector<uint8_t> data) {
    std::cout << "receive data on connection " << id << " size " << data.size()
              << std::endl;
  }

  TcpServerTask(IP ip, Port port) : ip_(ip), port_(port){};
  State run(Loop *) override {
    std::cout << "tcp server start in " << ip_ << ":" << port_ << std::endl;
    return State::Finish;
  }

private:
  IP ip_;
  Port port_;
  std::vector<ConnectId> connctions_;
};

} // namespace mygo
