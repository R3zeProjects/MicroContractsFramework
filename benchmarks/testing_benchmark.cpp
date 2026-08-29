#include <vosp/testing.hpp>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string_view>

namespace
{
using Clock = std::chrono::steady_clock;
inline std::atomic_uint64_t benchmark_sink{0};

#if defined(_MSC_VER)
#define VOSP_BENCH_NOINLINE __declspec(noinline)
#else
#define VOSP_BENCH_NOINLINE __attribute__((noinline))
#endif

[[nodiscard]] VOSP_BENCH_NOINLINE bool consume(const std::uint64_t value,
                                               std::uint64_t &checksum) noexcept
{
    checksum ^= value + 0x9E3779B97F4A7C15ULL + (checksum << 6U) + (checksum >> 2U);
    std::atomic_signal_fence(std::memory_order_seq_cst);
    return true;
}

[[nodiscard]] VOSP_BENCH_NOINLINE std::uint64_t generate(vosp::testing::Random &random) noexcept
{
    return random.next();
}

template <typename Operation>
void measure(const std::string_view scenario, const std::size_t operations, Operation operation)
{
    for (std::size_t sample = 0; sample < 5; ++sample)
    {
        const auto started = Clock::now();
        const auto report = operation(sample);
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - started).count();
        if (!report.passed())
        {
            std::abort();
        }
        const auto per_operation = static_cast<double>(elapsed) / static_cast<double>(operations);
        const auto throughput = 1'000'000'000.0 / per_operation;
        std::cout << scenario << ',' << sample << ',' << operations << ',' << elapsed << ','
                  << std::fixed << std::setprecision(3) << per_operation << ','
                  << std::setprecision(0) << throughput << '\n';
    }
}
} // namespace

int main()
{
    constexpr std::size_t sequential_operations = 5'000'000;
    constexpr std::size_t concurrent_operations = 1'000'000;
    std::cout << "scenario,sample,operations,total_ns,ns_per_operation,operations_per_second\n";

    measure("stress_single", sequential_operations, [](const std::size_t sample) {
        std::uint64_t checksum = sample + 1;
        auto report = vosp::testing::run_stress(
            sequential_operations, [&](const std::size_t index) {
                return consume(static_cast<std::uint64_t>(index), checksum);
            });
        benchmark_sink.store(checksum, std::memory_order_relaxed);
        return report;
    });

    measure("property_single", sequential_operations, [](const std::size_t sample) {
        std::uint64_t checksum = 0;
        auto report = vosp::testing::check_property(
            {.cases = sequential_operations, .seed = sample + 1},
            [](vosp::testing::Random &random, std::size_t) { return generate(random); },
            [&](const std::uint64_t value) { return consume(value, checksum); });
        benchmark_sink.store(checksum, std::memory_order_relaxed);
        return report;
    });

    const auto concurrent = []<std::size_t Workers>(const std::string_view name) {
        measure(name, concurrent_operations, [](std::size_t) {
            std::atomic_size_t calls{0};
            auto report = vosp::testing::run_concurrently(
                {.workers = Workers,
                 .iterations_per_worker = concurrent_operations / Workers,
                 .stop_on_failure = true},
                [&](std::size_t, std::size_t, const std::stop_token &) {
                    calls.fetch_add(1, std::memory_order_relaxed);
                    return true;
                });
            benchmark_sink.store(calls.load(std::memory_order_relaxed),
                                 std::memory_order_relaxed);
            return report;
        });
    };
    concurrent.template operator()<1>("concurrency_1");
    concurrent.template operator()<4>("concurrency_4");
    concurrent.template operator()<8>("concurrency_8");
}

#undef VOSP_BENCH_NOINLINE
