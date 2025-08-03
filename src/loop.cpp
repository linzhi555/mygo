#include "loop.h"

#include <uv.h>

#include <cstring>
namespace mygo {

Task::State TimerTask::run(Loop* loop) {
  if (!inited_) {
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

    inited_ = true;
    return State::NotFinish;
  }

  if (finished_) return State::Finish;
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

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

void on_write(uv_write_t* req, int status) {
  if (status < 0) {
    fprintf(stderr, "Write error: %s\n", uv_strerror(status));
  }
  free(req);
}

void send_response(uv_stream_t* client) {
  // HTTP 响应内容
  const char* response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 12\r\n"
      "\r\n"
      "Hello World\n";
  // 创建写请求
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  uv_buf_t buf = uv_buf_init((char*)response, strlen(response));
  // 发送响应
  uv_write(req, client, &buf, 1, on_write);
}
void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  if (nread > 0) {
    // 解析 HTTP 请求（此处简单打印请求）
    printf("Received request:\n%.*s", (int)nread, buf->base);
    send_response(client);
  }

  free(buf->base);  // 无论成功与否都要释放缓冲区

  if (nread < 0) {
    if (nread != UV_EOF) {
      fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
    }
    uv_close((uv_handle_t*)client, (uv_close_cb)free);
  }
}

void on_new_connection(uv_stream_t* server, int status) {
  auto* tcp_server = static_cast<TcpServerTask*>(server->data);
  uv_loop_t* loop = tcp_server->uv_loop_;
  if (status < 0) {
    fprintf(stderr, "New connection error: %s\n", uv_strerror(status));
    return;
  }
  uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  if (uv_accept(server, (uv_stream_t*)client) == 0) {
    uv_read_start((uv_stream_t*)client, alloc_buffer, on_read);
  } else {
    uv_close((uv_handle_t*)client, (uv_close_cb)free);
  }
}

Task::State TcpServerTask::run(Loop* loop) {
  uv_loop_ = loop->uv_loop_;
  if (err_) return State::Error;

  if (!inited_) {
    std::cout << "initialize tcp server task in " << ip_ << ":" << port_
              << std::endl;

    uv_tcp_init(loop->uv_loop_, &server_);
    uv_ip4_addr(ip_.c_str(), port_, &addr_);
    int rc = uv_tcp_bind(&server_, (const struct sockaddr*)&addr_, 0);
    if (rc != 0) {
      fprintf(stderr, "bind ip error%s\n", uv_strerror(rc));
      err_ = true;
      return State::Error;
    }

    server_.data = this;

    rc = uv_listen((uv_stream_t*)&server_, 128, on_new_connection);
    if (rc != 0) {
      fprintf(stderr, "Listen error: %s\n", uv_strerror(rc));
      err_ = true;
      return State::Error;
    }

    inited_ = true;
  }

  return State::NotFinish;
}

}  // namespace mygo
