#include <vosp/testing.hpp>

#include <atomic>
#include <cstddef>

int main()
{
    const auto property = vsp::testing::check_property(
        {.cases = 10'000, .seed = 0xC0FFEE},
        [](vsp::testing::Random &random, std::size_t) { return random.uniform(0, 1'000); },
        [](const auto value) { return value <= 1'000; });

    std::atomic_size_t operations{0};
    const auto concurrent = vsp::testing::run_concurrently(
        {.workers = 4, .iterations_per_worker = 1'000},
        [&](std::size_t, std::size_t, const std::stop_token &) {
            operations.fetch_add(1, std::memory_order_relaxed);
            return true;
        });

    return property.passed() && concurrent.passed() && operations == 4'000 ? 0 : 1;
}
