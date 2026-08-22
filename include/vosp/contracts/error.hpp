#pragma once

/** @file error.hpp Shared, implementation-independent error contracts. */

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
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

/** @brief Result of a recoverable operation. */
template <typename T> using Result = std::expected<T, Error>;

/** @brief Result of a recoverable operation without a value. */
using OperationResult = Result<void>;

} // namespace vosp::error
