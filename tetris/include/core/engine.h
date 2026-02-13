#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>
#include <cstdint>
#include <thread>

constexpr const double INITIAL_GRAVITY_INTERVAL      = 0.7; // seconds
constexpr const double INITIAL_GRAVITY_FACTOR        = 1.0; // seconds
constexpr const double BASE_SOFT_DROP_GRAVITY_FACTOR = 0.3; // seconds

using clock_type                  = std::chrono::steady_clock;
using seconds_f                   = std::chrono::duration<float>;
constexpr std::uint8_t TARGET_FPS = 30;
constexpr auto FRAME_TIME = std::chrono::duration<double>(1.0 / TARGET_FPS);

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
