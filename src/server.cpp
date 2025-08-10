#include "server.h"

#include <uv.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "channel.h"
#include "loop/loop.h"
#include "loop/task.h"

namespace mygo {

const int MAX_THREAD = 1;
int count = 0;

int Server::Run() {
  running_ = true;

  std::vector<Loop> loops;
  std::vector<std::thread> threads;

  for (int i = 0; i < MAX_THREAD; ++i) {
    loops.emplace_back(std::string("Loop" + std::to_string(i)));
  }

  int i = 0;
  for (auto& loop : loops) {
    std::string id = loop.id();
    loop.PostTask([i, id](Loop* this_loop) {
      std::cout << id << " " << i << std::endl;
      this_loop->PostTimerTask(
          true,
          [](Loop* loop) {
            count++;
            std::cout << "count is " << " " << count << std::endl;
            if (count == 1) {
              loop->PostTask(std::unique_ptr<Task>(new TcpClientTask(
                  std::string("127.0.0.1"), 8888, std::string("hello world"),
                  [](std::string response) {
                    std::cout << response << std::endl;
                  })));
              std::cout << "boot client task" << std::endl;
            }
          },
          Duration((i + 1) * Sec + 1 * Sec));
    });

    loop.PostTask(std::unique_ptr<Task>(new TcpServerTask("127.0.0.1", 8888)));
    i++;

    threads.emplace_back([](Loop* loop) { loop->run(); }, &loop);
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  running_ = false;
  return 0;
}

int Server::Stop() {
  running_ = false;
  return 0;
}

}  // namespace mygo
