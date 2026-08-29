#pragma once

/** @file property.hpp Deterministic property-test runner. */

#include <vosp/testing/random.hpp>
#include <vosp/testing/report.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace vosp::testing
{
/** @brief Reproducible property-run parameters. */
struct PropertyOptions
{
    std::size_t cases = 100;
    std::uint64_t seed = 0;
};

/**
 * @brief Checks generated values until all cases pass or the first case fails.
 * @param options Case count and seed used to reproduce a failing run.
 * @param generator Callable `(Random&, size_t) -> Value`.
 * @param property Callable `(const Value&) -> bool`.
 */
template <typename Generator, typename Property>
    requires std::invocable<Generator &, Random &, std::size_t> &&
             std::predicate<Property &,
                            const std::invoke_result_t<Generator &, Random &, std::size_t> &>
[[nodiscard]] TestReport check_property(const PropertyOptions options, Generator &&generator,
                                        Property &&property)
{
    TestReport report{};
    report.scheduled = options.cases;
    Random random{options.seed};

    for (std::size_t index = 0; index < options.cases; ++index)
    {
        try
        {
            auto value = std::invoke(generator, random, index);
            ++report.completed;
            if (!std::invoke(property, std::as_const(value)))
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
