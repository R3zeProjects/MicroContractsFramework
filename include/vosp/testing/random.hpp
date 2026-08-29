#pragma once

/** @file random.hpp Small deterministic generator for reproducible tests. */

#include <cstdint>
#include <stdexcept>

namespace vosp::testing
{
/** @brief Deterministic SplitMix64 generator with unbiased bounded sampling. */
class Random final
{
public:
    /** @brief Constructs a reproducible stream from @p seed. */
    explicit constexpr Random(const std::uint64_t seed) noexcept : state_{seed} {}

    /** @return The next 64-bit value in the stream. */
    [[nodiscard]] constexpr std::uint64_t next() noexcept
    {
        auto value = (state_ += 0x9E3779B97F4A7C15ULL);
        value = (value ^ (value >> 30U)) * 0xBF58476D1CE4E5B9ULL;
        value = (value ^ (value >> 27U)) * 0x94D049BB133111EBULL;
        return value ^ (value >> 31U);
    }

    /** @return An unbiased value in the inclusive range [lower, upper]. */
    [[nodiscard]] constexpr std::uint64_t uniform(const std::uint64_t lower,
                                                  const std::uint64_t upper)
    {
        if (lower > upper)
        {
            throw std::invalid_argument{"Random::uniform requires lower <= upper"};
        }

        const auto range = upper - lower + 1U;
        if (range == 0U)
        {
            return next();
        }

        const auto threshold = (0U - range) % range;
        auto value = next();
        while (value < threshold)
        {
            value = next();
        }
        return lower + value % range;
    }

private:
    std::uint64_t state_;
};
} // namespace vosp::testing
