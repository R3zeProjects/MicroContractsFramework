#pragma once

/** @file protocol.hpp Compile-time contracts for protocol components. */

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include <vosp/contracts/error.hpp>

namespace vosp::contracts
{
/** @brief Version value exposed by a protocol message. */
template<typename Type>
concept ProtocolVersion = std::regular<Type> && requires(const Type& version, const Type& peer)
{
    { version.major() } -> std::convertible_to<std::uint16_t>;
    { version.minor() } -> std::convertible_to<std::uint16_t>;
    { version.compatible_with(peer) } -> std::same_as<bool>;
};

/** @brief Immutable message view accepted at framing boundaries. */
template<typename Type>
concept ProtocolMessage = requires(const Type& message)
{
    typename Type::VersionType;
    requires ProtocolVersion<typename Type::VersionType>;
    { message.version() } -> std::convertible_to<typename Type::VersionType>;
    { message.type() } -> std::convertible_to<std::uint32_t>;
    { message.correlation_id() } -> std::convertible_to<std::uint64_t>;
    { message.payload() } -> std::same_as<std::span<const std::byte>>;
};

/** @brief Replaceable value codec using the shared error model. */
template<typename Codec, typename Value, typename Model>
concept ProtocolCodec = ErrorModel<Model> && requires(
    Codec& codec, const Value& value, std::span<const std::byte> bytes)
{
    { codec.encode(value) } ->
        std::same_as<typename Model::template Result<std::vector<std::byte>>>;
    { codec.decode(bytes) } -> std::same_as<typename Model::template Result<Value>>;
};

/** @brief Replaceable message-to-frame codec. */
template<typename Framer, typename Message, typename Model>
concept ProtocolFramer = ErrorModel<Model> && ProtocolMessage<Message> &&
    requires(Framer& framer, const Message& message, std::span<const std::byte> bytes)
{
    { framer.encode(message) } ->
        std::same_as<typename Model::template Result<std::vector<std::byte>>>;
    { framer.decode(bytes) } -> std::same_as<typename Model::template Result<Message>>;
};

/** @brief Incremental decoder used by stream-oriented transports. */
template<typename Decoder, typename Message, typename Model>
concept ProtocolStreamDecoder = ErrorModel<Model> && ProtocolMessage<Message> &&
    requires(Decoder& decoder, std::span<const std::byte> bytes)
{
    { decoder.push(bytes) } -> std::same_as<typename Model::OperationResult>;
    { decoder.next() } ->
        std::same_as<typename Model::template Result<std::optional<Message>>>;
    { decoder.buffered_size() } -> std::convertible_to<std::size_t>;
    { decoder.reset() } -> std::same_as<void>;
};
} // namespace vosp::contracts
