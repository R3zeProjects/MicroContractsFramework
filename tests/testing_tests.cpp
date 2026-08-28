#include <vosp/testing.hpp>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace
{
[[nodiscard]] bool random_is_reproducible()
{
    vosp::testing::Random left{42};
    vosp::testing::Random right{42};
    for (std::size_t index = 0; index < 128; ++index)
    {
        if (left.next() != right.next())
        {
            return false;
        }
    }
    return true;
}

[[nodiscard]] bool property_reports_first_failure()
{
    const auto passed = vosp::testing::check_property(
        {.cases = 1'000, .seed = 7},
        [](vosp::testing::Random &random, std::size_t) { return random.uniform(0, 99); },
        [](const std::uint64_t value) { return value < 100; });
    const auto failed = vosp::testing::check_property(
        {.cases = 10, .seed = 7},
        [](vosp::testing::Random &, const std::size_t index) { return index; },
        [](const std::size_t value) { return value < 4; });
    return passed.passed() && !failed.passed() && failed.completed == 5 &&
           failed.first_failure && failed.first_failure->iteration == 4;
}

[[nodiscard]] bool stress_reports_exceptions()
{
    const auto report = vosp::testing::run_stress(8, [](const std::size_t index) {
        if (index == 3)
        {
            throw std::runtime_error{"expected test exception"};
        }
        return true;
    });
    return !report.passed() && report.completed == 4 && report.failures == 1 &&
           report.first_exception && report.first_failure &&
           report.first_failure->iteration == 3;
}

[[nodiscard]] bool concurrency_is_joined_and_bounded()
{
    std::atomic_size_t calls{0};
    const auto report = vosp::testing::run_concurrently(
        {.workers = 4, .iterations_per_worker = 2'000, .stop_on_failure = true},
        [&](std::size_t, std::size_t, const std::stop_token &) {
            calls.fetch_add(1, std::memory_order_relaxed);
            return true;
        });

    bool upper_bound_rejected = false;
    try
    {
        static_cast<void>(vosp::testing::run_concurrently(
            {.workers = vosp::testing::max_worker_count + 1, .iterations_per_worker = 1},
            [](std::size_t, std::size_t, const std::stop_token &) { return true; }));
    }
    catch (const std::invalid_argument &)
    {
        upper_bound_rejected = true;
    }

    bool zero_rejected = false;
    try
    {
        static_cast<void>(vosp::testing::run_concurrently(
            {.workers = 0, .iterations_per_worker = 1},
            [](std::size_t, std::size_t, const std::stop_token &) { return true; }));
    }
    catch (const std::invalid_argument &)
    {
        zero_rejected = true;
    }
    return report.passed() && report.completed == 8'000 && calls == 8'000 &&
           upper_bound_rejected && zero_rejected;
}

[[nodiscard]] bool concurrency_cancels_and_captures_exceptions()
{
    const auto cancelled = vosp::testing::run_concurrently(
        {.workers = 4, .iterations_per_worker = 100, .stop_on_failure = true},
        [](std::size_t, std::size_t, const std::stop_token &) { return false; });
    const auto continued = vosp::testing::run_concurrently(
        {.workers = 2, .iterations_per_worker = 10, .stop_on_failure = false},
        [](const std::size_t worker, const std::size_t iteration,
           const std::stop_token &) {
            if (worker == 1 && iteration == 5)
            {
                throw std::runtime_error{"expected concurrent exception"};
            }
            return !(worker == 0 && iteration == 3);
        });

    return !cancelled.passed() && cancelled.failures >= 1 &&
           cancelled.completed <= 4 && !continued.passed() &&
           continued.completed == 20 && continued.failures == 2 &&
           continued.first_exception;
}
} // namespace

int main()
{
    return random_is_reproducible() && property_reports_first_failure() &&
                   stress_reports_exceptions() && concurrency_is_joined_and_bounded() &&
                   concurrency_cancels_and_captures_exceptions()
               ? 0
               : 1;
}
