#pragma once

/** @file error.hpp Compile-time contracts for replaceable error models. */

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace vosp::contracts
{
/** @brief Requirements imposed on an owning error implementation. */
template <typename Type>
concept Error = std::copy_constructible<Type> && std::movable<Type> &&
                requires(const Type &error) {
                    { error.code() } -> std::convertible_to<std::uint32_t>;
                    { error.message() } -> std::convertible_to<std::string_view>;
                };

/** @brief Minimal expected-like result protocol used at framework boundaries. */
template <typename Type, typename ErrorType>
concept Result = requires(Type &result, const Type &const_result) {
    { const_result.has_value() } -> std::same_as<bool>;
    { const_result.error() } -> std::convertible_to<const ErrorType &>;
    { result.error() } -> std::convertible_to<ErrorType &>;
};

/**
 * @brief Provider contract for a replaceable error/result implementation.
 *
 * A model owns the concrete Error and Result implementations. MCF only checks
 * their public protocol and never constructs a framework implementation itself.
 */
template <typename Model>
concept ErrorModel = requires(std::uint32_t code, std::string message) {
    typename Model::Error;
    typename Model::template Result<int>;
    typename Model::OperationResult;
    requires Error<typename Model::Error>;
    requires Result<typename Model::template Result<int>, typename Model::Error>;
    requires Result<typename Model::OperationResult, typename Model::Error>;
    { Model::make_error(code, std::move(message)) } -> std::same_as<typename Model::Error>;
};
} // namespace vosp::contracts
