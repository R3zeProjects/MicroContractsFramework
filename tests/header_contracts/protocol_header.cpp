#include <vosp/contracts/protocol.hpp>

#include <cstddef>
#include <compare>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
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

class Version
{
public:
    [[nodiscard]] std::uint16_t major() const noexcept { return 1; }
    [[nodiscard]] std::uint16_t minor() const noexcept { return 0; }
    [[nodiscard]] bool compatible_with(const Version&) const noexcept { return true; }
    auto operator<=>(const Version&) const = default;
};

class Message
{
public:
    using VersionType = Version;
    [[nodiscard]] Version version() const noexcept { return {}; }
    [[nodiscard]] std::uint32_t type() const noexcept { return 1; }
    [[nodiscard]] std::uint64_t correlation_id() const noexcept { return 2; }
    [[nodiscard]] std::span<const std::byte> payload() const noexcept { return {}; }
};

struct Codec
{
    [[nodiscard]] Model::Result<std::vector<std::byte>> encode(const std::string&)
    {
        return std::vector<std::byte>{};
    }
    [[nodiscard]] Model::Result<std::string> decode(std::span<const std::byte>)
    {
        return std::string{};
    }
};

struct Framer
{
    [[nodiscard]] Model::Result<std::vector<std::byte>> encode(const Message&)
    {
        return std::vector<std::byte>{};
    }
    [[nodiscard]] Model::Result<Message> decode(std::span<const std::byte>)
    {
        return Message{};
    }
};

struct StreamDecoder
{
    [[nodiscard]] Model::OperationResult push(std::span<const std::byte>) { return {}; }
    [[nodiscard]] Model::Result<std::optional<Message>> next() { return std::nullopt; }
    [[nodiscard]] std::size_t buffered_size() const noexcept { return 0; }
    void reset() noexcept {}
};

static_assert(vosp::contracts::ProtocolVersion<Version>);
static_assert(vosp::contracts::ProtocolMessage<Message>);
static_assert(vosp::contracts::ProtocolCodec<Codec, std::string, Model>);
static_assert(vosp::contracts::ProtocolFramer<Framer, Message, Model>);
static_assert(vosp::contracts::ProtocolStreamDecoder<StreamDecoder, Message, Model>);
} // namespace
