#include <vosp/contracts/plugin.hpp>

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

struct Error
{
    [[nodiscard]] std::uint32_t code() const noexcept { return 1; }
    [[nodiscard]] std::string_view message() const noexcept { return "error"; }
};

struct Model
{
    using Error = ::Error;
    template <typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;

    [[nodiscard]] static Error make_error(std::uint32_t, std::string) { return {}; }
};

struct InvalidFactory
{
    // Missing plugin_type and owning create result by design.
};

static_assert(vosp::contracts::PluginFactory<InvalidFactory, Model>,
              "an incomplete factory must not satisfy PluginFactory");

int main() {}
