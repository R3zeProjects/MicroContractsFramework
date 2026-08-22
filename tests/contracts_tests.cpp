#include <vosp/contracts.hpp>

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace
{
class TestError
{
  public:
    TestError(std::uint32_t code, std::string message)
        : code_{code}, message_{std::move(message)}
    {
    }

    [[nodiscard]] std::uint32_t code() const noexcept { return code_; }
    [[nodiscard]] std::string_view message() const noexcept { return message_; }

  private:
    std::uint32_t code_;
    std::string message_;
};

struct TestErrorModel
{
    using Error = TestError;
    template <typename Type> using Result = std::expected<Type, Error>;
    using OperationResult = Result<void>;

    [[nodiscard]] static Error make_error(std::uint32_t code, std::string message)
    {
        return Error{code, std::move(message)};
    }
};

enum class TestLevel
{
    info,
    error
};

struct TestEntry
{
    TestLevel level = TestLevel::info;
    TestError error{0, {}};
};

struct TestSink
{
    [[nodiscard]] bool write(const TestEntry &) { return true; }
};

struct InvalidError
{
};

static_assert(vosp::contracts::Error<TestError>);
static_assert(!vosp::contracts::Error<InvalidError>);
static_assert(vosp::contracts::ErrorModel<TestErrorModel>);
static_assert(vosp::contracts::LogEntry<TestEntry>);
static_assert(vosp::contracts::LogSink<TestSink, TestEntry>);
} // namespace

int main()
{
    const auto error = TestErrorModel::make_error(42, "replaceable implementation");
    TestSink sink;
    const bool accepted = sink.write(TestEntry{.level = TestLevel::error, .error = error});
    return error.code() == 42 && error.message() == "replaceable implementation" && accepted
               ? 0
               : 1;
}
