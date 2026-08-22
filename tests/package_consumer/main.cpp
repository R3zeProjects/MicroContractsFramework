#include <vosp/contracts.hpp>

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace
{
struct Error
{
    [[nodiscard]] std::uint32_t code() const noexcept { return value; }
    [[nodiscard]] std::string_view message() const noexcept { return text; }

    std::uint32_t value = 0;
    std::string text;
};

struct Model
{
    using Error = ::Error;
    template <typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;

    [[nodiscard]] static Error make_error(std::uint32_t code, std::string message)
    {
        return Error{code, std::move(message)};
    }
};

static_assert(vosp::contracts::ErrorModel<Model>);
} // namespace

int main()
{
    return Model::make_error(7, "consumer").code() == 7 ? 0 : 1;
}

