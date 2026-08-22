#pragma once

/** @file logging.hpp Shared logging boundary for ecosystem components. */

#include <vosp/contracts/error.hpp>

#include <chrono>
#include <concepts>
#include <cstdint>
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
