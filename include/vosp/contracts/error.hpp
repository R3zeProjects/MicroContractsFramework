#pragma once

/** @file error.hpp Shared, implementation-independent error contracts. */

#include <concepts>
#include <cstdint>
#include <exception>
#include <expected>
#include <format>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace vosp::error
{
/** @brief Coarse operational categories shared by the VOSP ecosystem. */
enum class Category : std::uint16_t
{
    NETWORK = 0,
    DATABASE,
    FILESYSTEM,
    NONE = 0xffff
};

/** @brief Returns the stable public name of an error category. */
[[nodiscard]] constexpr std::string_view category_name(Category category) noexcept
{
    switch (category)
    {
    case Category::NETWORK:
        return "NETWORK";
    case Category::DATABASE:
        return "DATABASE";
    case Category::FILESYSTEM:
        return "FILESYSTEM";
    case Category::NONE:
        return "NONE";
    }
    return "UNKNOWN";
}

/** @brief Owning error value shared across independently deployable frameworks. */
class Error
{
  public:
    explicit Error(Category category, std::uint32_t code, std::string message)
        : message_{std::move(message)}, code_{code}, category_{category}
    {
    }

    /** @brief Compares all owned fields; C++ synthesizes operator!=. */
    [[nodiscard]] bool operator==(const Error &) const noexcept = default;

    [[nodiscard]] std::uint32_t code() const noexcept { return code_; }
    [[nodiscard]] std::string_view message() const noexcept { return message_; }
    [[nodiscard]] Category category() const noexcept { return category_; }
    [[nodiscard]] bool has_category() const noexcept { return category_ != Category::NONE; }

  private:
    std::string message_;
    std::uint32_t code_;
    Category category_;
};

/** @brief Formats an error as `[CATEGORY:code] message`. */
[[nodiscard]] inline std::string to_string(const Error &error)
{
    return std::format("[{}:{}] {}", category_name(error.category()), error.code(),
                       error.message());
}

/** @brief Hash function matching Error equality. */
struct ErrorHash
{
    [[nodiscard]] std::size_t operator()(const Error &error) const noexcept
    {
        const auto code_hash = std::hash<std::uint32_t>{}(error.code());
        const auto message_hash = std::hash<std::string_view>{}(error.message());
        const auto category_hash =
            std::hash<std::uint16_t>{}(static_cast<std::uint16_t>(error.category()));
        return code_hash ^
               (message_hash + 0x9e3779b9u + (code_hash << 6u) + (code_hash >> 2u)) ^
               (category_hash << 1u);
    }
};

/** @brief Result of a recoverable operation. */
template <typename T> using Result = std::expected<T, Error>;

/** @brief Result of a recoverable operation without a value. */
using OperationResult = Result<void>;

/**
 * @brief Invokes a callable and translates thrown exceptions into an Error.
 * @note Allocation failure while constructing the diagnostic may propagate.
 */
template <typename Function>
    requires std::invocable<Function &&> &&
             (!std::is_reference_v<std::invoke_result_t<Function &&>>)
[[nodiscard]] auto attempt(Error failure, Function &&function)
    -> Result<std::invoke_result_t<Function &&>>
{
    using Value = std::invoke_result_t<Function &&>;
    try
    {
        if constexpr (std::is_void_v<Value>)
        {
            std::invoke(std::forward<Function>(function));
            return {};
        }
        else
        {
            return std::invoke(std::forward<Function>(function));
        }
    }
    catch (const std::exception &exception)
    {
        std::string message{failure.message()};
        const char *const diagnostic = exception.what();
        if (const std::string_view detail =
                diagnostic != nullptr ? std::string_view{diagnostic} : std::string_view{};
            !detail.empty())
        {
            if (!message.empty())
            {
                message.append(": ");
            }
            message.append(detail);
        }
        return std::unexpected(
            Error{failure.category(), failure.code(), std::move(message)});
    }
    catch (...)
    {
        return std::unexpected(std::move(failure));
    }
}
} // namespace vosp::error

