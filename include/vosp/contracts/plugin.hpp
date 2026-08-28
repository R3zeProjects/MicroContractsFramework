#pragma once

/** @file plugin.hpp Compile-time contracts for replaceable plugin components. */

#include <vosp/contracts/error.hpp>

#include <concepts>
#include <memory>
#include <string_view>

namespace vosp::contracts
{
/** @brief Named plugin instance with explicit start and stop operations. */
template <typename Type, typename Model>
concept PluginLifecycle = ErrorModel<Model> && std::destructible<Type> &&
    requires(Type &plugin, const Type &immutable)
{
    { immutable.name() } -> std::convertible_to<std::string_view>;
    { immutable.version() } -> std::convertible_to<std::string_view>;
    { plugin.start() } -> std::same_as<typename Model::OperationResult>;
    { plugin.stop() } -> std::same_as<typename Model::OperationResult>;
};

/** @brief Factory that creates one owning plugin instance or a model error. */
template <typename Factory, typename Model>
concept PluginFactory = requires
{
    typename Factory::plugin_type;
} && PluginLifecycle<typename Factory::plugin_type, Model> && requires(Factory &factory)
{
    { factory.create() } ->
        std::same_as<typename Model::template Result<std::unique_ptr<typename Factory::plugin_type>>>;
};
} // namespace vosp::contracts
