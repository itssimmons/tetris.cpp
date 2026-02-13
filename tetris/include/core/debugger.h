#pragma once

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class Debugger
{
  public:
    void start(const std::string& filename);
    void stop();
    void log(std::string msg);

  private:
    void process();

    std::ofstream out;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> queue;
    std::atomic<bool> running{false};
};

#endif // DEBUGGER_H
