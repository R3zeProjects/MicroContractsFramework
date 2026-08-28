#include <vosp.hpp>
#include <vosp/configuration.hpp>
#include <vosp/contracts.hpp>
#include <vosp/persistence.hpp>
#include <vosp/protocol.hpp>
#include <vosp/resilience.hpp>
#include <vosp/security.hpp>
#include <vosp/service.hpp>
#include <vosp/telemetry.hpp>
#include <vosp/transport.hpp>
#include <vosp/workflow.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace
{
static_assert(vosp::contracts::version::major == 0);
static_assert(vosp::protocol::version::api == "0.3.0-beta");
static_assert(vosp::transport::version::api == "0.3.0-beta");
static_assert(vosp::security::version::api == "0.3.0-beta");
static_assert(vosp::contracts::ByteStreamTransport<vosp::transport::TcpStream,
                                                   vosp::transport::Model>);
static_assert(vosp::version::minor == 6);
static_assert(vosp::persistence::version::minor == 4);
static_assert(vosp::telemetry::version::patch >= 1);
static_assert(vosp::configuration::version::minor == 1);
static_assert(vosp::resilience::version::minor == 1);
static_assert(vosp::workflow::version_minor == 1);
static_assert(vosp::service::version == "0.1.0-beta");
static_assert(vosp::contracts::ErrorModel<vosp::error::Model>);
static_assert(vosp::contracts::KeyValueCache<vsp::Cache<std::string, std::string>>);

class MemoryJournal
{
  public:
    [[nodiscard]] vosp::persistence::OperationResult append(
        const vosp::persistence::Record &record)
    {
        std::scoped_lock lock{mutex_};
        records_.push_back(record);
        return {};
    }

    [[nodiscard]] std::size_t size() const
    {
        std::scoped_lock lock{mutex_};
        return records_.size();
    }

  private:
    mutable std::mutex mutex_;
    std::vector<vosp::persistence::Record> records_;
};

class RejectingJournal
{
  public:
    [[nodiscard]] vosp::persistence::OperationResult append(
        const vosp::persistence::Record &)
    {
        return std::unexpected{
            vosp::persistence::Error{0xE001, "deliberate ecosystem rejection"}};
    }
};

class PersistenceExporter final : public vosp::telemetry::IExporter
{
  public:
    explicit PersistenceExporter(MemoryJournal &journal) noexcept : journal_{&journal} {}

    [[nodiscard]] bool export_batch(
        std::span<const vosp::telemetry::Record> records) override
    {
        for (const auto &record : records)
        {
            if (!journal_->append({
                    .timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                        record.timestamp().time_since_epoch())
                                        .count(),
                    .message = std::string{record.name()},
                }))
            {
                return false;
            }
        }
        return true;
    }

  private:
    MemoryJournal *journal_;
};

template <typename Pipeline> class ConfigurationObserver
{
  public:
    explicit ConfigurationObserver(Pipeline &pipeline) noexcept : pipeline_{&pipeline} {}

    void configuration_changed(
        const std::shared_ptr<const vosp::configuration::Snapshot> &snapshot)
    {
        published_ = pipeline_->publish(vosp::telemetry::Record::event(
            "configuration.changed",
            {{"revision", std::to_string(snapshot->revision())}}));
    }

    [[nodiscard]] bool published() const noexcept { return published_; }

  private:
    Pipeline *pipeline_;
    bool published_{false};
};

[[nodiscard]] bool validate_process_and_resilience(
    const std::filesystem::path &executable)
{
    vsp::ProcessWorkflow<vsp::error::Model> workflow;
    vosp::ProcessSpec process{.executable = executable,
                              .arguments = {"--ecosystem-child"},
                              .working_directory = std::nullopt};
    if (!workflow.add({"ecosystem-smoke", std::move(process), {}}))
    {
        return false;
    }

    vsp::Policy<vsp::error::Model> resilience;
    const auto result = resilience.execute([&] { return workflow.run(); });
    if (!result || !result->succeeded())
    {
        return false;
    }

    vosp::ProcessRunner<vsp::error::Model> runner;
    const auto failed_process = runner.run({.executable = executable,
                                            .arguments = {"--ecosystem-child-fail"},
                                            .working_directory = std::nullopt});
    if (!failed_process || failed_process->succeeded() || failed_process->exit_code != 7)
    {
        return false;
    }

    vsp::Retry<vsp::error::Model> retry{{.max_attempts = 3,
                                         .backoff = vsp::Backoff::none,
                                         .initial_delay = {},
                                         .max_delay = {}}};
    std::uint16_t attempts{};
    const auto retried = retry.execute(
        [&]() -> vosp::error::Result<int>
        {
            if (++attempts < 3)
            {
                return std::unexpected{vosp::error::Error{
                    vosp::error::Category::NETWORK, 503, "temporary failure"}};
            }
            return 42;
        });
    return retried && *retried == 42 && attempts == 3;
}

[[nodiscard]] bool validate_data_plane()
{
    MemoryJournal journal;
    auto exporter = std::make_shared<PersistenceExporter>(journal);
    vosp::TelemetryPipeline<vosp::telemetry::pipeline_policy::Direct> pipeline{exporter};
    vosp::persistence::Sink sink{journal};
    vosp::logger::Logger logger{sink};
    vosp::telemetry::Registry metrics;
    auto changes = metrics.counter("configuration.changes");

    vosp::Configuration configuration;
    vosp::configuration::Layer layer{"runtime"};
    layer.set("service.generation", std::int64_t{1});
    const std::array layers{layer};
    ConfigurationObserver observer{pipeline};
    const vosp::error::Error error{
        vosp::error::Category::DATABASE, 1001, "configuration changed"};

    if (!configuration.publish_and_notify(layers, observer) || !observer.published() ||
        !logger.error(error) || !changes.add() || !pipeline.flush())
    {
        return false;
    }

    const auto stats = pipeline.stats();
    return configuration.snapshot()->revision() == 1 && changes.value() == 1 &&
           sink.accepted() == 1 && stats.exported == 1 && journal.size() == 2;
}

[[nodiscard]] bool validate_protocol_plane()
{
    vosp::protocol::Utf8Codec text;
    auto payload = text.encode(std::string{"ecosystem.protocol"});
    if (!payload)
    {
        return false;
    }

    vsp::Protocol protocol;
    auto frame = protocol.encode(vsp::ProtocolMessage{
        vsp::ProtocolVersion{1, 0}, 7, 42, std::move(*payload)});
    if (!frame)
    {
        return false;
    }

    const auto decoded = protocol.decode(*frame);
    if (!decoded || decoded->type() != 7 || decoded->correlation_id() != 42)
    {
        return false;
    }
    const auto message = text.decode(decoded->payload());
    return message && *message == "ecosystem.protocol";
}

[[nodiscard]] bool validate_cache_plane()
{
    vsp::Cache<std::string, int> cache{2};
    cache.put("alpha", 1);
    auto snapshot = cache.get("alpha");
    if (!snapshot)
    {
        return false;
    }
    *snapshot = 99;
    const auto retained = cache.get("alpha");
    if (!retained || *retained != 1)
    {
        return false;
    }

    cache.put("beta", 2);
    cache.put("gamma", 3);
    const auto beta = cache.get("beta");
    const auto gamma = cache.get("gamma");
    const auto stats = cache.stats();
    return !cache.contains("alpha") && beta && *beta == 2 && gamma && *gamma == 3 &&
           cache.size() == 2 && stats.insertions == 3 && stats.evictions == 1;
}

[[nodiscard]] bool validate_async_data_plane()
{
    MemoryJournal journal;
    auto exporter = std::make_shared<PersistenceExporter>(journal);
    using Async = vosp::telemetry::pipeline_policy::Async<16, 4>;
    vosp::TelemetryPipeline<Async> pipeline{exporter};
    constexpr std::size_t record_count = 64;
    for (std::size_t index = 0; index < record_count; ++index)
    {
        if (!pipeline.publish(vosp::telemetry::Record::event(
                "ecosystem.async", {{"index", std::to_string(index)}})))
        {
            return false;
        }
    }

    if (!pipeline.flush())
    {
        return false;
    }
    pipeline.shutdown();
    const auto stats = pipeline.stats();
    return stats.accepted == record_count && stats.exported == record_count &&
           journal.size() == record_count &&
           !pipeline.publish(vosp::telemetry::Record::event("late"));
}

[[nodiscard]] bool validate_failure_paths()
{
    RejectingJournal rejecting_journal;
    vosp::persistence::Sink rejecting_sink{rejecting_journal};
    vosp::logger::Logger logger{rejecting_sink};
    const bool rejected_log = !logger.error(vosp::error::Error{
        vosp::error::Category::FILESYSTEM, 5, "persistence unavailable"});

    vosp::Configuration configuration;
    vosp::configuration::Schema schema;
    schema.require<std::int64_t>("service.port",
                                 [](std::int64_t port) { return port > 0 && port <= 65535; });
    vosp::configuration::Layer valid{"valid"};
    valid.set("service.port", std::int64_t{8080});
    const std::array valid_layers{valid};
    if (!configuration.publish(valid_layers, schema))
    {
        return false;
    }

    vosp::configuration::Layer invalid{"invalid"};
    invalid.set("service.port", "not-an-integer");
    const std::array invalid_layers{invalid};
    const auto rejected_configuration = configuration.publish(invalid_layers, schema);
    return rejected_log && rejecting_sink.failed() == 1 &&
           !rejected_configuration && configuration.snapshot()->revision() == 1;
}

[[nodiscard]] bool validate_service_control_plane()
{
    using Result = vosp::error::OperationResult;
    std::vector<std::string> events;
    vsp::Services<vsp::error::Model> services;
    const auto database = services.add({
        .name = "database",
        .dependencies = {},
        .start = [&]() -> Result
        {
            events.emplace_back("start:database");
            return {};
        },
        .stop = [&]() -> Result
        {
            events.emplace_back("stop:database");
            return {};
        },
        .health = [] { return vsp::Health::healthy; }});
    const auto api = services.add({
        .name = "api",
        .dependencies = {"database"},
        .start = [&]() -> Result
        {
            events.emplace_back("start:api");
            return {};
        },
        .stop = [&]() -> Result
        {
            events.emplace_back("stop:api");
            return {};
        },
        .health = [] { return vsp::Health::healthy; }});
    if (!database || !api || !services.start_all())
    {
        return false;
    }
    const auto status = services.snapshot();
    if (status.size() != 2 || !services.stop_all())
    {
        return false;
    }
    return events == std::vector<std::string>{"start:database", "start:api", "stop:api",
                                               "stop:database"};
}

[[nodiscard]] bool validate_transport_plane()
{
    vosp::transport::TcpStream stream;
    vosp::transport::UdpSocket socket;
    const auto invalid_connect = stream.connect(vosp::transport::IpEndpoint{});
    const auto invalid_receive = socket.receive(0);
    return !stream.connected() && !socket.open() && !invalid_connect && !invalid_receive &&
           invalid_connect.error().kind() == vosp::transport::ErrorCode::invalid_argument &&
           invalid_receive.error().kind() == vosp::transport::ErrorCode::not_connected;
}
} // namespace

int main(int argc, char **argv)
{
    if (argc > 1 && std::string_view{argv[1]} == "--ecosystem-child")
    {
        return 0;
    }
    if (argc > 1 && std::string_view{argv[1]} == "--ecosystem-child-fail")
    {
        return 7;
    }

    return validate_process_and_resilience(std::filesystem::absolute(argv[0])) &&
                   validate_data_plane() && validate_protocol_plane() &&
                   validate_cache_plane() && validate_async_data_plane() &&
                   validate_failure_paths() &&
                   validate_service_control_plane() && validate_transport_plane()
               ? 0
               : 1;
}
