#pragma once

/** @file stress.hpp Deterministic single-thread stress runner. */

#include <vosp/testing/report.hpp>

#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>

namespace vosp::testing
{
/**
 * @brief Executes an indexed operation until completion or its first failure.
 * @param iterations Maximum number of calls.
 * @param operation Callable `(size_t) -> bool`.
 */
template <typename Operation>
    requires std::predicate<Operation &, std::size_t>
[[nodiscard]] TestReport run_stress(const std::size_t iterations, Operation &&operation)
{
    TestReport report{};
    report.scheduled = iterations;
    for (std::size_t index = 0; index < iterations; ++index)
    {
        try
        {
            const bool passed = std::invoke(operation, index);
            ++report.completed;
            if (!passed)
            {
                report.failures = 1;
                report.first_failure = FailureLocation{.worker = 0, .iteration = index};
                break;
            }
        }
        catch (...)
        {
            ++report.completed;
            report.failures = 1;
            report.first_failure = FailureLocation{.worker = 0, .iteration = index};
            report.first_exception = std::current_exception();
            break;
        }
    }
    return report;
}
} // namespace vosp::testing
