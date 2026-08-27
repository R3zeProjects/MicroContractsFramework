#include <vosp/contracts/transport.hpp>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
class Error
{
public:
    Error(std::uint32_t code, std::string message)
        : code_{code}, message_{std::move(message)}
    {
    }

    [[nodiscard]] std::uint32_t code() const noexcept { return code_; }
    [[nodiscard]] std::string_view message() const noexcept { return message_; }

private:
    std::uint32_t code_;
    std::string message_;
};

struct Model
{
    using Error = ::Error;
    template<typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;

    [[nodiscard]] static Error make_error(std::uint32_t code, std::string message)
    {
        return {code, std::move(message)};
    }
};

struct Endpoint
{
};

struct Stream
{
    [[nodiscard]] bool connected() const noexcept { return true; }
    [[nodiscard]] Model::Result<std::size_t> send(std::span<const std::byte>)
    {
        return 0;
    }
    [[nodiscard]] Model::Result<std::size_t> receive(std::span<std::byte>)
    {
        return 0;
    }
    [[nodiscard]] Model::OperationResult connect(const Endpoint&) { return {}; }
    [[nodiscard]] Model::OperationResult reconnect() { return {}; }
    void close() noexcept {}
};

class Datagram
{
public:
    using EndpointType = Endpoint;
    [[nodiscard]] const Endpoint& endpoint() const noexcept { return endpoint_; }
    [[nodiscard]] std::span<const std::byte> payload() const noexcept { return payload_; }

private:
    Endpoint endpoint_;
    std::vector<std::byte> payload_;
};

struct DatagramSocket
{
    [[nodiscard]] Model::Result<std::size_t> send_to(
        const Endpoint&, std::span<const std::byte>)
    {
        return 0;
    }
    [[nodiscard]] Model::Result<Datagram> receive() { return Datagram{}; }
    void close() noexcept {}
};

struct InvalidStream
{
    [[nodiscard]] int connected() const noexcept { return 1; }
};

static_assert(vosp::contracts::ByteStreamTransport<Stream, Model>);
static_assert(vosp::contracts::TransportConnector<Stream, Endpoint, Model>);
static_assert(vosp::contracts::TransportDatagram<Datagram>);
static_assert(vosp::contracts::DatagramTransport<DatagramSocket, Endpoint, Datagram, Model>);
static_assert(!vosp::contracts::ByteStreamTransport<InvalidStream, Model>);
} // namespace
