#pragma once
#include <uv.h>

#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>

#include "common.h"

namespace mygo {
class Loop;

class Task {
 public:
  class State {
   public:
    State() : internal_(0) {}
    State(int state) : internal_(state) {}

    static State Error() { return State(-2); }
    static State Finished() { return State(-1); }

    bool operator==(const State& other) { return is(other); }

    bool isS0() { return internal_ == 0; }
    bool isS1() { return internal_ == 1; }
    bool isS2() { return internal_ == 2; }
    bool isS3() { return internal_ == 3; }
    bool is(const State& other) { return this->internal_ == other.internal_; }
    bool isAfter(int state) { return state > internal_; }
    bool isError() { return internal_ < -1; }
    bool isFinished() { return internal_ == -1; }
    bool isNotFinished() { return internal_ >= 0; }
    void setError() { internal_ = -2; }
    void setFinished() { internal_ = -1; }

    void next() {
      assert(internal_ >= 0);
      internal_++;
    }

   private:
    int internal_;
  };

  virtual ~Task() {};

  virtual State run(Loop* l) = 0;
};

class RunOnce : public Task {
 public:
  RunOnce(std::function<void(Loop*)> f) { func_ = f; }
  State run(Loop* l) override {
    if (state_.isFinished()) {
      return state_;
    }
    if (state_.isS0()) {
      func_(l);
      state_.setFinished();
      return state_;
    }

    assert("should not reach here" && false);
  }
  ~RunOnce() override = default;

 protected:
  State state_;
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
  State state_;
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

  virtual void OnData(ConnectId id, std::string data) {
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World\n";

    std::cout << "receive data on connection " << id << " size " << data
              << std::endl;

    std::cout << data << std::endl;

    SendData(id, response);
  }

  void SendData(ConnectId id, std::string data);

  State run(Loop* loop) override;
  TcpServerTask(IP ip, Port port) : ip_(ip), port_(port) {};

  uv_loop_t* uv_loop_;

 private:
  struct ConnectEvent {
    ConnectId id;
  };

  struct DataEvent {
    ConnectId id;
    std::string data;
  };

  static void on_new_connection(uv_stream_t* server, int status);
  static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
  int new_connection_id() {
    cur_connction_id_++;
    return cur_connction_id_;
  }

  State state_;
  IP ip_;
  Port port_;
  struct sockaddr_in addr_;
  uv_tcp_t server_;
  int cur_connction_id_ = -1;
  std::unordered_map<uv_tcp_t*, int> connection_id_map_;
  std::unordered_map<int, uv_tcp_t*> connection_handle_map_;
  std::optional<ConnectEvent> new_connect_;
  std::optional<DataEvent> new_data_;
};

// TODO: implement TcpClientTask
class TcpClientTask : public Task {
 public:
  TcpClientTask(IP ip, Port port, std::string need_send,
                std::function<void(std::string response)> call_back)
      : ip_(ip), port_(port), need_send_(need_send), call_back_(call_back) {};
  virtual ~TcpClientTask() {};

  State run(Loop* loop) override;

  State state_;
  IP ip_;
  Port port_;

  uv_tcp_t client;
  std::string need_send_;
  std::string response_;
  std::function<void(std::string response)> call_back_;
};

}  // namespace mygo
