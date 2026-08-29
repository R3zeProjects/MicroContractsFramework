#pragma once

/** @file report.hpp Common reports produced by VOSP test utilities. */

#include <cstddef>
#include <exception>
#include <optional>

namespace vosp::testing
{
/** @brief Identifies one failed operation in a deterministic test run. */
struct FailureLocation
{
    std::size_t worker = 0;
    std::size_t iteration = 0;
};

/** @brief Allocation-free summary of a property, stress, or concurrency run. */
struct TestReport
{
    std::size_t scheduled = 0;
    std::size_t completed = 0;
    std::size_t failures = 0;
    std::optional<FailureLocation> first_failure;
    std::exception_ptr first_exception;

    /** @return True only when every scheduled operation completed successfully. */
    [[nodiscard]] bool passed() const noexcept
    {
        return completed == scheduled && failures == 0 && !first_exception;
    }
};
} // namespace vosp::testing
