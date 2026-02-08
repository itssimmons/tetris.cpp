#include <chrono>
#include <random>
#include <vector>

class RNG
{
  public:
    RNG(const std::vector<uint32_t>& seeds = {})
    {
        if (seeds.empty())
        {
            auto t = std::chrono::high_resolution_clock::now()
                         .time_since_epoch()
                         .count();
            engine.seed(static_cast<uint64_t>(t));
        }
        else
        {
            std::seed_seq seq(seeds.begin(), seeds.end());
            engine.seed(seq);
        }
    }

    uint32_t operator()()
    {
        return engine(); // produces 32-bit random value
    }

  private:
    std::mt19937 engine;
};

inline size_t fastRange(uint32_t r, size_t range)
{
    return (uint64_t(r) * uint64_t(range)) >> 32;
}

inline size_t randIndex(RNG& rng, size_t maxExclusive)
{
    return fastRange(rng(), maxExclusive);
}
