#pragma once

/** @file configuration.hpp Compile-time contracts for configuration providers. */

#include <concepts>
#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

namespace vosp::contracts
{
/** @brief Minimal immutable configuration snapshot protocol. */
template<typename Type>
concept ConfigurationSnapshot = requires(const Type& snapshot, std::string_view key)
{
    { snapshot.revision() } -> std::convertible_to<std::uint64_t>;
    { snapshot.contains(key) } -> std::same_as<bool>;
};

/** @brief Provider protocol for atomically published immutable snapshots. */
template<typename Type>
concept ConfigurationProvider = requires(const Type& provider)
{
    typename Type::Snapshot;
    requires ConfigurationSnapshot<typename Type::Snapshot>;
    { provider.snapshot() } ->
        std::same_as<std::shared_ptr<const typename Type::Snapshot>>;
};

/** @brief Observer protocol used at configuration composition boundaries. */
template<typename Type, typename Snapshot>
concept ConfigurationObserver = ConfigurationSnapshot<Snapshot> && requires(
    Type& observer, std::shared_ptr<const Snapshot> snapshot)
{
    { observer.configuration_changed(std::move(snapshot)) } -> std::same_as<void>;
};
} // namespace vosp::contracts
