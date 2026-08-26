# Ecosystem comparison

## Comparison policy

The VOSP ecosystem is not one monolithic replacement for every project in this
document. Each framework has a deliberately narrow responsibility. A numerical
comparison is published only when the repository contains a reproducible target
that performs the same delivered work, validates the result, pins the compared
release, and runs every scenario as a separate Release process. Feature-only
comparisons are not converted into performance rankings.

The reference run below used an AMD Ryzen 7 PRO 1700X (8 cores, 16 logical
processors), 32 GiB RAM, Windows 10 Pro 19045, Clang 22.1.6 for the C++ targets
except the MSVC 19.51 process comparison, CMake 4.3.2, and seven samples. Values
are medians; they are evidence for this machine and workload, not cross-machine
guarantees.

## Measured comparisons

| Framework and scenario | VOSP median | Compared median | Semantic boundary |
| --- | ---: | ---: | --- |
| MEF synchronous file logging, 1 producer | 2.977M records/s | spdlog 1.17.0: 1.777M records/s | Same owned records, formatted bytes, final drain, and file-byte validation |
| MEF synchronous file logging, 4 producers | 5.017M records/s | spdlog 1.17.0: 1.193M records/s | Same shared file workload; not a comparison of every spdlog sink or formatter |
| MEF asynchronous file logging, 1 producer | 3.059M records/s | Quill 12.1.0: 0.587M records/s | Same messages and delivered file bytes; Quill's broader frontend/backend feature set is outside the workload |
| MEF asynchronous file logging, 4 producers | 2.390M records/s | Quill 12.1.0: 0.549M records/s | Same bounded delivery check; not a universal latency claim |
| MPF buffered append | 0.759M records/s | SQLite 3.53.4 WAL `NORMAL`, one batch: 0.339M records/s | MPF performs an OS-buffer flush; SQLite performs a transactional WAL commit |
| MPF buffered append | 0.759M records/s | SQLite 3.53.4 WAL `FULL`, one batch: 0.308M records/s | Durability semantics differ and are reported explicitly |
| MTF counter update | 124.499M updates/s | prometheus-cpp 1.3.0: 135.966M; OpenTelemetry C++ 1.9.1: 25.920M | One in-process counter update; exporters and protocol work are excluded |
| MTF histogram observation | 56.930M observations/s | prometheus-cpp 1.3.0: 24.664M; OpenTelemetry C++ 1.9.1: 27.106M | One observation with result-count validation |
| MConfigF typed lookup | 29.410M lookups/s | toml++ 3.4.0: 19.842M | Pre-parsed lookup of the same integer value |
| MConfigF parser | 1.239M documents/s | toml++ 3.4.0: 0.154M | Not grammar-equivalent: MConfigF parses a smaller `key=value` language; toml++ implements TOML |
| MWF verified child-process round trip | 28.14 launches/s supervised; 26.58 launch-only | Boost.Process 2.0: 26.82; libuv 1.52.1: 28.31 | Same executable, argument, wait, and verified zero exit code; OS startup dominates |

Raw alternating-order samples are stored in the owning repositories:

- [MEF logger samples](https://github.com/R3zeProjects/MicroErrorFramework/blob/main/benchmark-results/external-logger-raw-2026-08-26.csv)
- [MPF persistence samples](https://github.com/R3zeProjects/MicroPersistenceFramework/blob/main/benchmark-results/external-persistence-raw-2026-08-26.csv)
- [MTF telemetry samples](https://github.com/R3zeProjects/MicroTelemetryFramework/blob/main/benchmark-results/external-telemetry-raw-2026-08-26.csv)
- [MConfigF configuration samples](https://github.com/R3zeProjects/MicroConfigurationFramework/blob/main/benchmark-results/external-configuration-raw-2026-08-26.csv)
- [MWF process samples](https://github.com/R3zeProjects/MicroWorkflowFramework/blob/main/benchmark-results/external-process-raw-2026-08-26.csv)

## Capability boundaries

| VOSP framework | Comparable project | Where the established project is broader | Where the VOSP component is deliberately narrower or different |
| --- | --- | --- | --- |
| MCF | C++ language concepts | Language concepts are the underlying mechanism | MCF names ecosystem-specific structural protocols and has zero runtime behavior |
| MEF | [spdlog](https://github.com/gabime/spdlog), [Quill](https://github.com/odygrd/quill) | More sinks, formatters, filtering, ecosystem maturity, and specialized logging features | MEF combines owning typed errors, category registers, bounded workers, and logging under one error-control API |
| MPF | [SQLite](https://www.sqlite.org/wal.html) | Transactions, SQL, indexes, concurrency control, recovery, and configurable durability | MPF is an owning append journal/codec abstraction, not a database |
| MTF | [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp), [OpenTelemetry C++](https://github.com/open-telemetry/opentelemetry-cpp) | Prometheus exposition and OpenTelemetry's stable logs/metrics/traces specification and exporters | MTF provides small in-process records, instruments, registries, and bounded pipelines; it does not claim OTLP or specification compliance |
| MConfigF | [toml++](https://github.com/marzer/tomlplusplus) | Complete TOML parsing and serialization | MConfigF owns layering, typed schema validation, immutable snapshots, observers, and atomic publication for a smaller input grammar |
| MRF | [Resilience4j](https://resilience4j.readme.io/docs/getting-started) | Rate limiting, caches, async/reactive integrations, registries, metrics, and Java framework modules | MRF is a C++23 policy layer for retry, circuit breaker, bulkhead, and cooperative deadlines; no external runtime benchmark is claimed |
| MWF | [Boost.Process 2](https://www.boost.org/doc/libs/latest/libs/process/doc/html/index.html), [libuv](https://docs.libuv.org/en/latest/guide/processes.html) | Boost.Asio process composition and libuv's complete event-loop/I/O model | MWF adds a compact policy API for cancellation, deadlines, descendant ownership, and resource supervision |
| MSF | [POCO Subsystem](https://docs.pocoproject.org/current/Poco.Util.Subsystem.html), [Boost.Ext.DI](https://boost-ext.github.io/di/) | POCO provides a complete application framework and reconfiguration; Boost.Ext.DI constructs object graphs | MSF owns runtime dependency ordering, rollback, health, and reentrant lifecycle rejection; construction-time DI is out of scope |

## Conclusions allowed by the evidence

- The nine installed packages compose through their public CMake targets and
  MCF contracts without runtime-to-runtime core dependencies.
- The measured hot paths are competitive on the documented machine and exact
  workloads.
- MEF, MPF, MTF, MConfigF, and MWF have reproducible external comparison
  targets. MCF has no runtime path; MRF and MSF currently publish internal
  regression benchmarks and feature comparisons only.
- No result in this report establishes universal superiority, production tail
  latency, crash durability equivalence, or feature parity with the compared
  projects.
