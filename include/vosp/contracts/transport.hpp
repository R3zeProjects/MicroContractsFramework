#pragma once

/** @file transport.hpp Compile-time contracts for byte transports. */

#include <concepts>
#include <cstddef>
#include <span>

#include <vosp/contracts/error.hpp>

namespace vosp::contracts
{
/** @brief Connected ordered byte stream such as TCP or local IPC. */
template<typename Connection, typename Model>
concept ByteStreamTransport = ErrorModel<Model> &&
    requires(Connection& connection, std::span<const std::byte> output,
             std::span<std::byte> input)
{
    { connection.connected() } -> std::same_as<bool>;
    { connection.send(output) } ->
        std::same_as<typename Model::template Result<std::size_t>>;
    { connection.receive(input) } ->
        std::same_as<typename Model::template Result<std::size_t>>;
    { connection.close() } -> std::same_as<void>;
};

/** @brief Endpoint-aware connector with an explicit reconnect operation. */
template<typename Connector, typename Endpoint, typename Model>
concept TransportConnector = ErrorModel<Model> &&
    requires(Connector& connector, const Endpoint& endpoint)
{
    { connector.connect(endpoint) } -> std::same_as<typename Model::OperationResult>;
    { connector.reconnect() } -> std::same_as<typename Model::OperationResult>;
};

/** @brief Owning datagram value returned by a message-oriented transport. */
template<typename Datagram>
concept TransportDatagram = requires(const Datagram& datagram)
{
    typename Datagram::EndpointType;
    { datagram.endpoint() } ->
        std::same_as<const typename Datagram::EndpointType&>;
    { datagram.payload() } -> std::same_as<std::span<const std::byte>>;
};

/** @brief Message-oriented datagram transport such as UDP. */
template<typename Socket, typename Endpoint, typename Datagram, typename Model>
concept DatagramTransport = ErrorModel<Model> && TransportDatagram<Datagram> &&
    requires(Socket& socket, const Endpoint& endpoint,
             std::span<const std::byte> payload)
{
    { socket.send_to(endpoint, payload) } ->
        std::same_as<typename Model::template Result<std::size_t>>;
    { socket.receive() } -> std::same_as<typename Model::template Result<Datagram>>;
    { socket.close() } -> std::same_as<void>;
};
} // namespace vosp::contracts
