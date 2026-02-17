#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>
#include <cstdint>

using clock_type                  = std::chrono::steady_clock;
using seconds_f                   = std::chrono::duration<float>;
constexpr std::uint8_t TARGET_FPS = 30;
constexpr const auto FRAME_TIME =
    std::chrono::duration<double>(1.0 / TARGET_FPS);

class Engine
{
  public:
    bool running = true;

    static int run();
    void update(double dt);
    void render();

  private:
    clock_type::time_point previous = clock_type::now();
    void loop();
};

#endif // ENGINE_H
