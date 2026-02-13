#include "core/debugger.h"

void Debugger::start(const std::string& filename)
{
    running = true;
    out.open(filename, std::ios::out | std::ios::trunc);
    worker = std::thread(&Debugger::process, this);
}

void Debugger::stop()
{
    running = false;
    cv.notify_all();
    if (worker.joinable()) worker.join();
    out.close();
}

void Debugger::log(std::string msg)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(msg));
    }
    cv.notify_one();
}

void Debugger::process()
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
