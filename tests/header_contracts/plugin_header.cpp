#include <vosp/contracts/plugin.hpp>

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>

namespace
{
struct Error
{
    [[nodiscard]] std::uint32_t code() const noexcept { return 0; }
    [[nodiscard]] std::string_view message() const noexcept { return {}; }
};

struct Model
{
    using Error = ::Error;
    template <typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;
    [[nodiscard]] static Error make_error(std::uint32_t, std::string) { return {}; }
};

struct Plugin
{
    [[nodiscard]] std::string_view name() const noexcept { return "plugin"; }
    [[nodiscard]] std::string_view version() const noexcept { return "1.0.0"; }
    [[nodiscard]] Model::OperationResult start() { return {}; }
    [[nodiscard]] Model::OperationResult stop() { return {}; }
};

struct Factory
{
    [[nodiscard]] Model::Result<std::unique_ptr<Plugin>> create()
    {
        return std::make_unique<Plugin>();
    }
};

struct InvalidFactory
{
    [[nodiscard]] Plugin *create();
};

static_assert(vosp::contracts::PluginLifecycle<Plugin, Model>);
static_assert(vosp::contracts::PluginFactory<Factory, Plugin, Model>);
static_assert(!vosp::contracts::PluginFactory<InvalidFactory, Plugin, Model>);
} // namespace
