#include <vosp/contracts.hpp>

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

struct Error
{
    std::uint32_t value = 0;
    std::string text;

    [[nodiscard]] std::uint32_t code() const noexcept { return value; }
    [[nodiscard]] std::string_view message() const noexcept { return text; }
};

struct ErrorModel
{
    using Error = ::Error;
    template <typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;

    [[nodiscard]] static Error make_error(std::uint32_t code, std::string message)
    {
        return {code, std::move(message)};
    }
};

static_assert(vosp::contracts::ErrorModel<ErrorModel>);

int main()
{
    const auto error = ErrorModel::make_error(7, "contract-compatible error");
    return error.code() == 7 ? 0 : 1;
}
