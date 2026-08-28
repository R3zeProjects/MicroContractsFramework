#pragma once

/** @file cache.hpp Structural contract for replaceable bounded caches. */

#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

namespace vosp::contracts
{
/**
 * @brief Owning key-value cache with explicit capacity and expiration cleanup.
 * @details `get` returns a value copy, `put` inserts or replaces, and
 * `purge_expired` returns the number of entries removed by that call.
 */
template <typename Cache>
concept KeyValueCache =
    requires(Cache &cache, const Cache &immutable, const typename Cache::key_type &key,
             typename Cache::mapped_type value) {
        typename Cache::key_type;
        typename Cache::mapped_type;
        { cache.get(key) } -> std::same_as<std::optional<typename Cache::mapped_type>>;
        { cache.put(key, std::move(value)) } -> std::same_as<void>;
        { cache.contains(key) } -> std::same_as<bool>;
        { cache.erase(key) } -> std::same_as<bool>;
        { cache.size() } -> std::same_as<std::size_t>;
        { immutable.capacity() } -> std::same_as<std::size_t>;
        { cache.purge_expired() } -> std::same_as<std::size_t>;
        { cache.clear() } -> std::same_as<void>;
    };
} // namespace vosp::contracts
