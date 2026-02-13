#include <cstdint>

#include "core/rng.h"

RNG::RNG(const std::vector<uint32_t>& seeds)
{
    if (seeds.empty())
    {
        auto t = std::chrono::high_resolution_clock::now()
                     .time_since_epoch()
                     .count();
        engine.seed(static_cast<uint32_t>(t));
    }
    else
    {
        std::seed_seq seq(seeds.begin(), seeds.end());
        engine.seed(seq);
    }
}

std::uint32_t RNG::operator()()
{
    return engine(); // produces 32-bit random value
}

size_t fastRange(std::uint32_t r, size_t range)
{
    return (uint64_t(r) * uint64_t(range)) >> 32;
}

namespace rng
{
size_t randIndex(RNG& rng, size_t maxExclusive)
{
    return fastRange(rng(), maxExclusive);
}
} // namespace rng
