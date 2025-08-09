#include "loop/task.h"

#include "loop/loop.h"
namespace mygo {

void alloc_buffer(uv_handle_t* _, size_t suggested_size, uv_buf_t* buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

void on_write(uv_write_t* req, int status) {
  if (status < 0) {
    fprintf(stderr, "Write error: %s\n", uv_strerror(status));
  }
  free(req);
}

void TcpServerTask::on_read(uv_stream_t* client, ssize_t nread,
                            const uv_buf_t* buf) {
  if (nread > 0) {
    auto* tcp_server = static_cast<TcpServerTask*>(client->data);
    int id = tcp_server->connection_id_map_[(uv_tcp_t*)(client)];

    tcp_server->new_data_ = DataEvent{id, std::string(buf->base)};
  }

  free(buf->base);  // 无论成功与否都要释放缓冲区

  if (nread < 0) {
    if (nread != UV_EOF) {
      fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
    }
    uv_close((uv_handle_t*)client, (uv_close_cb)free);
  }
}

void TcpServerTask::on_new_connection(uv_stream_t* server, int status) {
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
    client->data = tcp_server;

    ConnectId new_id = tcp_server->new_connection_id();
    tcp_server->connection_id_map_[client] = new_id;
    tcp_server->connection_handle_map_[new_id] = client;
    tcp_server->new_connect_ = ConnectEvent{new_id};

  } else {
    uv_close((uv_handle_t*)client, (uv_close_cb)free);
  }
}

static void send_response(uv_stream_t* client, const char* response) {
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  uv_buf_t buf = uv_buf_init((char*)response, strlen(response));
  uv_write(req, client, &buf, 1, on_write);
}

void TcpServerTask::SendData(ConnectId id, std::string data) {
  uv_stream_t* stream = (uv_stream_t*)connection_handle_map_[id];
  send_response(stream, data.c_str());
}

// HTTP 响应内容

Task::State TcpServerTask::run(Loop* loop) {
  uv_loop_ = loop->uv_loop_;
  if (state_.isError()) return state_;

  if (state_.isInitial()) {
    std::cout << "initialize tcp server task in " << ip_ << ":" << port_
              << std::endl;

    uv_tcp_init(loop->uv_loop_, &server_);
    uv_ip4_addr(ip_.c_str(), port_, &addr_);
    int rc = uv_tcp_bind(&server_, (const struct sockaddr*)&addr_, 0);
    if (rc != 0) {
      fprintf(stderr, "bind ip error%s\n", uv_strerror(rc));
      state_.setError();
      return state_;
    }

    server_.data = this;

    rc = uv_listen((uv_stream_t*)&server_, 128, on_new_connection);
    if (rc != 0) {
      fprintf(stderr, "Listen error: %s\n", uv_strerror(rc));
      state_.setError();
      return state_;
    }

    state_.next();
    return state_;
  }

  if (new_connect_) {
    OnConnect(*loop, new_connect_->id);
    new_connect_ = std::nullopt;
    return state_;
  }

  if (new_data_) {
    OnData(new_data_->id, new_data_->data);
    new_data_ = std::nullopt;
    return state_;
  }

  return state_;
}

Task::State TcpClientTask::run(Loop* _) { return state_; }

}  // namespace mygo
