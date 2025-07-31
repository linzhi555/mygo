#include "server.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <uv.h>

#include "channel.h"
#include "loop.h"

namespace mygo {

const int MAX_THREAD = 1;

int Server::Run() {
  running_ = true;

  std::vector<Loop> loops;
  std::vector<std::thread> threads;

  for (int i = 0; i < MAX_THREAD; ++i) {
    loops.emplace_back(std::string("Loop" + std::to_string(i)));
  }

  int i = 0;
  for (auto &loop : loops) {
    std::string id = loop.id();
    loop.PostTask([i, id](Loop *this_loop) {
      std::this_thread::sleep_for(Duration(1000));
      std::cout << id << " " << i << std::endl;
      this_loop->PostTimerTask(
          true,
          [i](Loop *) {
            std::cout << "its a callback after" << i << std::endl;
          },
          Duration((i + 1) * 1000));
    });
    i++;

    threads.emplace_back([](Loop *loop) { loop->run(); }, &loop);
  }

  for (std::thread &thread : threads) {
    thread.join();
  }

  running_ = false;
  return 0;
}

int Server::Stop() {
  running_ = false;
  return 0;
}

} // namespace mygo
