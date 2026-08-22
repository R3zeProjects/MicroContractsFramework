#pragma once

/** @file logging.hpp Compile-time contracts for log entries and sinks. */

#include <vosp/contracts/error.hpp>

#include <concepts>
#include <type_traits>

namespace vosp::contracts
{
/** @brief Requirements imposed on a structured log-entry implementation. */
template <typename Type>
concept LogEntry = requires(const Type &entry) {
    requires Error<std::remove_cvref_t<decltype(entry.error)>>;
    entry.level;
};

/** @brief Structural sink protocol; inheritance is not required. */
template <typename Sink, typename Entry>
concept LogSink = LogEntry<Entry> && requires(Sink &sink, const Entry &entry) {
    { sink.write(entry) } -> std::same_as<bool>;
};
} // namespace vosp::contracts
