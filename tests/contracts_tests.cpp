#include <vosp/contracts.hpp>

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <span>
#include <chrono>

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

struct TestTelemetryRecord
{
    [[nodiscard]] std::string_view name() const noexcept { return metric_name; }
    [[nodiscard]] std::chrono::system_clock::time_point timestamp() const noexcept
    {
        return captured_at;
    }

    std::string metric_name;
    std::chrono::system_clock::time_point captured_at{};
};

struct TestExporter
{
    [[nodiscard]] bool export_batch(std::span<const TestTelemetryRecord>)
    {
        return true;
    }
};

struct TestConfigurationSnapshot
{
    [[nodiscard]] std::uint64_t revision() const noexcept { return 7; }

    [[nodiscard]] bool contains(std::string_view key) const noexcept
    {
        return key == "service.port";
    }
};

struct TestConfigurationProvider
{
    using Snapshot = TestConfigurationSnapshot;

    [[nodiscard]] std::shared_ptr<const Snapshot> snapshot() const
    {
        return std::make_shared<const Snapshot>();
    }
};

struct TestConfigurationObserver
{
    void configuration_changed(
        std::shared_ptr<const TestConfigurationSnapshot> snapshot)
    {
        last_revision = snapshot->revision();
    }

    std::uint64_t last_revision = 0;
};

static_assert(vosp::contracts::Error<TestError>);
static_assert(!vosp::contracts::Error<InvalidError>);
static_assert(vosp::contracts::ErrorModel<TestErrorModel>);
static_assert(vosp::contracts::LogEntry<TestEntry>);
static_assert(vosp::contracts::LogSink<TestSink, TestEntry>);
static_assert(vosp::contracts::TelemetryRecord<TestTelemetryRecord>);
static_assert(vosp::contracts::TelemetryExporter<TestExporter, TestTelemetryRecord>);
static_assert(vosp::contracts::ConfigurationSnapshot<TestConfigurationSnapshot>);
static_assert(vosp::contracts::ConfigurationProvider<TestConfigurationProvider>);
static_assert(vosp::contracts::ConfigurationObserver<
              TestConfigurationObserver, TestConfigurationSnapshot>);
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
