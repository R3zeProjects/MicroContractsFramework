#include <vosp/contracts/telemetry.hpp>

#include <chrono>
#include <span>
#include <string>
#include <string_view>

namespace
{
struct Record
{
    [[nodiscard]] std::string_view name() const noexcept { return metric_name; }
    [[nodiscard]] std::chrono::system_clock::time_point timestamp() const noexcept
    {
        return captured_at;
    }

    std::string metric_name;
    std::chrono::system_clock::time_point captured_at{};
};

struct Exporter
{
    [[nodiscard]] bool export_batch(std::span<const Record>) { return true; }
};

static_assert(vosp::contracts::TelemetryRecord<Record>);
static_assert(vosp::contracts::TelemetryExporter<Exporter, Record>);
} // namespace
