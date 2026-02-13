#pragma once

#ifndef RNG_H
#define RNG_H

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

class RNG
{
  public:
    RNG(const std::vector<uint32_t>& seeds = {});

    uint32_t operator()();

  private:
    std::mt19937 engine;
};

size_t fastRange(uint32_t r, size_t range);

namespace rng
{
size_t randIndex(RNG& rng, size_t maxExclusive);
} // namespace rng

#endif // RNG_H
