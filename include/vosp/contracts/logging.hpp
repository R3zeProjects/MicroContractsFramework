#pragma once

/** @file logging.hpp Shared logging boundary for ecosystem components. */

#include <vosp/contracts/error.hpp>

#include <chrono>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <thread>
#include <type_traits>

namespace vosp::logger
{
/** @brief Severity of a log record. */
enum class Level : std::uint8_t
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/** @brief Converts a log level to its stable public name. */
[[nodiscard]] constexpr std::string_view to_string(Level level) noexcept
{
    switch (level)
    {
    case Level::TRACE:
        return "TRACE";
    case Level::DEBUG:
        return "DEBUG";
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARNING";
    case Level::ERROR:
        return "ERROR";
    case Level::CRITICAL:
        return "CRITICAL";
    }
    return "UNKNOWN";
}

/** @brief Owning log value exchanged directly between producers and sinks. */
struct LogEntry
{
    std::chrono::system_clock::time_point timestamp{};
    std::thread::id thread_id{};
    Level level = Level::INFO;
    vosp::error::Error error;
};

/** @brief Direct destination contract for one structured log record. */
class ILogSink
{
  public:
    [[nodiscard]] virtual bool write(const LogEntry &entry) = 0;
    virtual ~ILogSink() noexcept = default;
};

/** @brief Restricts a type to direct ILogSink implementations. */
template <typename Sink>
concept SinkType = std::derived_from<std::remove_cvref_t<Sink>, ILogSink>;
} // namespace vosp::logger

