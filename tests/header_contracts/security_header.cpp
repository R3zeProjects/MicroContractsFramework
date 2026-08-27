#include <vosp/contracts/security.hpp>

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

class Secret
{
  public:
    [[nodiscard]] std::size_t size() const noexcept { return bytes_.size(); }
    [[nodiscard]] std::span<std::byte> bytes() noexcept { return bytes_; }
    [[nodiscard]] std::span<const std::byte> bytes() const noexcept { return bytes_; }
    void clear() noexcept { bytes_.clear(); }

  private:
    std::vector<std::byte> bytes_;
};

struct Digest
{
    using DigestType = std::vector<std::byte>;
    [[nodiscard]] Model::Result<DigestType> digest(std::span<const std::byte>) const
    {
        return DigestType{};
    }
};

struct Authenticator
{
    using TagType = std::vector<std::byte>;
    [[nodiscard]] Model::Result<TagType> authenticate(std::span<const std::byte>) const
    {
        return TagType{};
    }
    [[nodiscard]] Model::OperationResult verify(std::span<const std::byte>,
                                                const TagType&) const
    {
        return {};
    }
};

enum class Permission : std::uint8_t
{
    read
};

struct Policy
{
    [[nodiscard]] bool allows(Permission, std::string_view) const noexcept { return true; }
};

struct InvalidSecret
{
    void clear() {}
};

static_assert(vosp::contracts::SecureBytes<Secret>);
static_assert(vosp::contracts::DigestProvider<Digest, Model>);
static_assert(vosp::contracts::MessageAuthenticator<Authenticator, Model>);
static_assert(vosp::contracts::PermissionPolicy<Policy, Permission, std::string_view>);
static_assert(!vosp::contracts::SecureBytes<InvalidSecret>);
} // namespace
