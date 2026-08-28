#pragma once

/** @file concurrency.hpp Bounded concurrent stress runner. */

#include <vosp/testing/report.hpp>

#include <atomic>
#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace vosp::testing
{
/** @brief Hard safety bound for a single concurrent test run. */
inline constexpr std::size_t max_worker_count = 1024;

/** @brief Parameters for a bounded concurrent test run. */
struct ConcurrencyOptions
{
    std::size_t workers = 1;
    std::size_t iterations_per_worker = 1;
    bool stop_on_failure = true;
};

/**
 * @brief Runs one shared operation concurrently and returns a joined summary.
 * @details The callable is shared by all workers and must therefore be safe for
 * concurrent invocation. Every worker receives its index, iteration, and a common
 * stop token. All created threads are joined before this function returns.
 */
template <typename Operation>
    requires std::predicate<Operation &, std::size_t, std::size_t, std::stop_token>
[[nodiscard]] TestReport run_concurrently(const ConcurrencyOptions options,
                                          Operation &&operation)
{
    if (options.workers == 0 || options.workers > max_worker_count)
    {
        throw std::invalid_argument{"worker count must be in [1, 1024]"};
    }
    if (options.iterations_per_worker >
        std::numeric_limits<std::size_t>::max() / options.workers)
    {
        throw std::overflow_error{"scheduled operation count overflows size_t"};
    }

    const auto scheduled = options.workers * options.iterations_per_worker;
    std::atomic_size_t completed{0};
    std::atomic_size_t failures{0};
    std::atomic_bool first_failure_claimed{false};
    std::atomic_bool start{false};
    FailureLocation first_failure{};
    std::exception_ptr first_exception;
    std::mutex exception_mutex;
    std::stop_source stop_source;
    std::vector<std::jthread> threads;
    threads.reserve(options.workers);

    const auto record_failure = [&](const std::size_t worker, const std::size_t iteration) {
        failures.fetch_add(1, std::memory_order_relaxed);
        bool expected = false;
        if (first_failure_claimed.compare_exchange_strong(
                expected, true, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            first_failure = FailureLocation{.worker = worker, .iteration = iteration};
        }
        if (options.stop_on_failure)
        {
            stop_source.request_stop();
        }
    };

    try
    {
        for (std::size_t worker = 0; worker < options.workers; ++worker)
        {
            threads.emplace_back([&, worker] {
                start.wait(false, std::memory_order_acquire);
                const auto token = stop_source.get_token();
                for (std::size_t iteration = 0; iteration < options.iterations_per_worker;
                     ++iteration)
                {
                    if (token.stop_requested())
                    {
                        break;
                    }
                    try
                    {
                        const bool passed = std::invoke(operation, worker, iteration, token);
                        completed.fetch_add(1, std::memory_order_relaxed);
                        if (!passed)
                        {
                            record_failure(worker, iteration);
                        }
                    }
                    catch (...)
                    {
                        completed.fetch_add(1, std::memory_order_relaxed);
                        {
                            const std::scoped_lock lock{exception_mutex};
                            if (!first_exception)
                            {
                                first_exception = std::current_exception();
                            }
                        }
                        record_failure(worker, iteration);
                    }
                }
            });
        }
    }
    catch (...)
    {
        stop_source.request_stop();
        start.store(true, std::memory_order_release);
        start.notify_all();
        throw;
    }

    start.store(true, std::memory_order_release);
    start.notify_all();
    threads.clear();

    TestReport report{};
    report.scheduled = scheduled;
    report.completed = completed.load(std::memory_order_relaxed);
    report.failures = failures.load(std::memory_order_relaxed);
    report.first_exception = first_exception;
    if (first_failure_claimed.load(std::memory_order_relaxed))
    {
        report.first_failure = first_failure;
    }
    return report;
}
} // namespace vosp::testing
