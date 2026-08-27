#pragma once

/** @file security.hpp Compile-time contracts for secret and cryptographic providers. */

#include <concepts>
#include <cstddef>
#include <span>

#include <vosp/contracts/error.hpp>

namespace vosp::contracts
{
/** @brief Mutable owning bytes whose contents can be explicitly erased. */
template<typename Buffer>
concept SecureBytes = requires(Buffer& buffer, const Buffer& immutable)
{
    { immutable.size() } -> std::same_as<std::size_t>;
    { immutable.bytes() } -> std::same_as<std::span<const std::byte>>;
    { buffer.bytes() } -> std::same_as<std::span<std::byte>>;
    { buffer.clear() } noexcept -> std::same_as<void>;
};

/** @brief Stateless or stateful provider of a fixed or variable-size digest. */
template<typename Provider, typename Model>
concept DigestProvider = ErrorModel<Model> &&
    requires(const Provider& provider, std::span<const std::byte> input)
{
    typename Provider::DigestType;
    { provider.digest(input) } ->
        std::same_as<typename Model::template Result<typename Provider::DigestType>>;
};

/** @brief Keyed authenticator that creates and verifies opaque tags. */
template<typename Authenticator, typename Model>
concept MessageAuthenticator = ErrorModel<Model> &&
    requires(const Authenticator& authenticator, std::span<const std::byte> input,
             const typename Authenticator::TagType& tag)
{
    typename Authenticator::TagType;
    { authenticator.authenticate(input) } ->
        std::same_as<typename Model::template Result<typename Authenticator::TagType>>;
    { authenticator.verify(input, tag) } -> std::same_as<typename Model::OperationResult>;
};

/** @brief Authorization policy independent of concrete permission/resource types. */
template<typename Policy, typename Permission, typename Resource>
concept PermissionPolicy = requires(const Policy& policy, Permission permission,
                                    const Resource& resource)
{
    { policy.allows(permission, resource) } -> std::same_as<bool>;
};
} // namespace vosp::contracts
