#pragma once
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class AsyncLogger
{
  public:
    void start(const std::string& filename)
    {
        running = true;
        out.open(filename, std::ios::out | std::ios::trunc);
        worker = std::thread(&AsyncLogger::process, this);
    }

    void stop()
    {
        running = false;
        cv.notify_all();
        if (worker.joinable()) worker.join();
        out.close();
    }

    void log(std::string msg)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(std::move(msg));
        }
        cv.notify_one();
    }

  private:
    void process()
    {
        std::unique_lock<std::mutex> lock(mtx);

        while (running || !queue.empty())
        {
            cv.wait(lock, [&] { return !running || !queue.empty(); });

            while (!queue.empty())
            {
                out << queue.front() << '\n';
                queue.pop();
            }

            out.flush(); // batch flush
        }
    }

    std::ofstream out;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> queue;
    std::atomic<bool> running{false};
};
