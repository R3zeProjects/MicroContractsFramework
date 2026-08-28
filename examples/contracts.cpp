#include <vosp/contracts.hpp>

#include <chrono>
#include <cstdint>
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>

struct Error {
  std::uint32_t value{};
  std::string text;
  [[nodiscard]] std::uint32_t code() const noexcept { return value; }
  [[nodiscard]] std::string_view message() const noexcept { return text; }
};

struct ErrorModel {
  using Error = ::Error;
  template <typename Type> using Result = std::expected<Type, Error>;
  using OperationResult = Result<void>;
  [[nodiscard]] static Error make_error(std::uint32_t code,
                                        std::string message) {
    return {code, std::move(message)};
  }
};

struct LogEntry {
  int level{};
  Error error{};
};

struct Sink {
  [[nodiscard]] bool write(const LogEntry &) { return true; }
};

struct TelemetryRecord {
  std::string metric_name{"requests"};
  [[nodiscard]] std::string_view name() const noexcept { return metric_name; }
  [[nodiscard]] std::chrono::system_clock::time_point
  timestamp() const noexcept {
    return {};
  }
};

struct Exporter {
  [[nodiscard]] bool export_batch(std::span<const TelemetryRecord>) {
    return true;
  }
};

struct Snapshot {
  [[nodiscard]] std::uint64_t revision() const noexcept { return 1; }
  [[nodiscard]] bool contains(std::string_view key) const noexcept {
    return key == "port";
  }
};

struct Provider {
  using Snapshot = ::Snapshot;
  [[nodiscard]] std::shared_ptr<const Snapshot> snapshot() const {
    return std::make_shared<const Snapshot>();
  }
};

struct Observer {
  void configuration_changed(
      std::shared_ptr<const Snapshot>
          value) // NOLINT(performance-unnecessary-value-param)
  {
    revision = value->revision();
  }
  std::uint64_t revision{};
};

struct Plugin {
  [[nodiscard]] std::string_view name() const noexcept { return "example.plugin"; }
  [[nodiscard]] std::string_view version() const noexcept { return "1.0.0"; }
  [[nodiscard]] ErrorModel::OperationResult start() { return {}; }
  [[nodiscard]] ErrorModel::OperationResult stop() { return {}; }
};

struct PluginFactory {
  [[nodiscard]] ErrorModel::Result<std::unique_ptr<Plugin>> create() {
    return std::make_unique<Plugin>();
  }
};

static_assert(vosp::contracts::Error<Error>);
static_assert(vosp::contracts::ErrorModel<ErrorModel>);
static_assert(vosp::contracts::LogEntry<LogEntry>);
static_assert(vosp::contracts::LogSink<Sink, LogEntry>);
static_assert(vosp::contracts::TelemetryRecord<TelemetryRecord>);
static_assert(vosp::contracts::TelemetryExporter<Exporter, TelemetryRecord>);
static_assert(vosp::contracts::ConfigurationSnapshot<Snapshot>);
static_assert(vosp::contracts::ConfigurationProvider<Provider>);
static_assert(vosp::contracts::ConfigurationObserver<Observer, Snapshot>);
static_assert(vosp::contracts::PluginLifecycle<Plugin, ErrorModel>);
static_assert(vosp::contracts::PluginFactory<PluginFactory, Plugin, ErrorModel>);

int main() {
  Provider provider;
  Observer observer;
  observer.configuration_changed(provider.snapshot());
  return observer.revision == 1 ? 0 : 1;
}
