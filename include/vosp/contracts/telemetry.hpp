#pragma once

/** @file telemetry.hpp Compile-time contracts for telemetry records and exporters. */

#include <chrono>
#include <concepts>
#include <span>
#include <string_view>

namespace vosp::contracts
{
/** @brief Requirements imposed on an owning telemetry record. */
template<typename Type>
concept TelemetryRecord = std::copy_constructible<Type> && std::movable<Type> &&
    requires(const Type& record)
    {
        { record.name() } -> std::convertible_to<std::string_view>;
        { record.timestamp() } ->
            std::convertible_to<std::chrono::system_clock::time_point>;
    };

/** @brief Structural batch exporter protocol; inheritance is not required. */
template<typename Exporter, typename Record>
concept TelemetryExporter = TelemetryRecord<Record> &&
    requires(Exporter& exporter, std::span<const Record> records)
    {
        { exporter.export_batch(records) } -> std::same_as<bool>;
    };
} // namespace vosp::contracts
