# MicroContractsFramework

MicroContractsFramework (MCF) is a header-only C++23 contract library for
building replaceable VOSP ecosystem components. It specifies compile-time API
requirements; it does not implement errors, results, loggers, sinks,
persistence, configuration storage, scheduling, or I/O.

MEF provides the standard production implementation of `Error`, `Result<T>`,
and logging types. MPF and future frameworks may accept that implementation or
any user-defined implementation satisfying the same MCF concepts.

## Why MCF exists

Frameworks depend on stable requirements instead of each other's concrete
classes:

```text
                 MicroContractsFramework
                 concepts / requirements
                    ^              ^
                    |              |
          MEF implementation    MPF templates
                    \              /
                     application
```

This provides compile-time dependency inversion without runtime registries,
wrapper objects, conversions, or adapter layers.

## Contracts

- `vosp::contracts::Error<T>` checks an owning error API;
- `vosp::contracts::Result<R, E>` checks an expected-like result API;
- `vosp::contracts::ErrorModel<M>` checks an error/result provider;
- `vosp::contracts::LogEntry<T>` checks a structured entry;
- `vosp::contracts::LogSink<S, E>` checks a structural sink implementation.
- `vosp::contracts::TelemetryRecord<T>` checks an owning telemetry value;
- `vosp::contracts::TelemetryExporter<X, T>` checks a batch exporter.
- `vosp::contracts::ConfigurationSnapshot<T>` checks an immutable snapshot;
- `vosp::contracts::ConfigurationProvider<T>` checks snapshot publication;
- `vosp::contracts::ConfigurationObserver<O, S>` checks change observers.
- `vosp::contracts::ProtocolMessage<T>` checks immutable message views;
- `vosp::contracts::ProtocolCodec<C, V, M>` checks value codecs;
- `vosp::contracts::ProtocolFramer<F, V, M>` checks message framing;
- `vosp::contracts::ProtocolStreamDecoder<D, V, M>` checks incremental decoding.
- `vosp::contracts::ByteStreamTransport<C, M>` checks ordered byte streams;
- `vosp::contracts::TransportConnector<C, E, M>` checks connect/reconnect behavior;
- `vosp::contracts::DatagramTransport<S, E, D, M>` checks datagram transports.
- `vosp::contracts::SecureBytes<B>` checks erasable secret ownership;
- `vosp::contracts::DigestProvider<P, M>` checks digest providers;
- `vosp::contracts::MessageAuthenticator<A, M>` checks keyed tag providers;
- `vosp::contracts::PermissionPolicy<P, A, R>` checks authorization policy.

MCF contains no concrete `Error`, `Result`, `LogEntry`, or `Sink` class.
Its performance contract is zero runtime work: all checks are concepts evaluated
at compile time. Negative tests now also reject sinks and telemetry exporters
whose return types do not satisfy the shared contracts.

## Custom implementation

```cpp
#include <vosp/contracts/error.hpp>

#include <expected>
#include <string>
#include <string_view>

struct MyError
{
    std::uint32_t code() const noexcept;
    std::string_view message() const noexcept;
};

struct MyErrorModel
{
    using Error = MyError;

    template<class T>
    using Result = std::expected<T, Error>;

    using OperationResult = Result<void>;

    static Error make_error(std::uint32_t code, std::string message);
};

static_assert(vosp::contracts::ErrorModel<MyErrorModel>);
```

MEF's implementation satisfies the same contract, so a framework parameterized
by `ErrorModel` can switch implementations without changing its algorithms.

## CMake

See the [installation guide](docs/INSTALLATION.md) for source-tree and installed
package workflows. Compilable examples and ecosystem composition patterns are
collected in [usage examples](docs/USAGE_EXAMPLES.md).
The [architecture note](docs/ARCHITECTURE.md) explains contract substitution,
dependency direction, and the zero-runtime operating model.
The [ecosystem compatibility matrix](docs/ECOSYSTEM_COMPATIBILITY.md) records
the package lines tested together and explains the Windows/Linux integration gate.
The [ecosystem comparison](docs/ECOSYSTEM_COMPARISON.md) separates reproducible
same-work benchmarks from feature-only comparisons with established projects.

```cmake
find_package(vosp_contracts 0.9 REQUIRED CONFIG)
target_link_libraries(your_target PRIVATE vosp::contracts)
```

Build and test:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Supported CI toolchains are GCC, Clang, and MSVC. MCF requires C++23 and has no
runtime or third-party dependency.

## Dependency rule

Dependencies may point to MCF. MCF never depends on MEF, MPF, or another
ecosystem implementation. Concrete behavior remains in the framework that owns
it or in the final application composition root.

## Ecosystem verification and language branches

MCF owns the ecosystem compatibility gate, not the runtime composition. The gate
installs all nine packages independently, rejects forbidden core dependencies,
and runs success, failure, retry, asynchronous drain, and child-process scenarios
through a downstream `find_package` consumer. See the
[compatibility matrix](docs/ECOSYSTEM_COMPATIBILITY.md).

`main` contains the canonical source and English documentation. `main-ru`
contains identical source code with Russian Markdown documentation. CI rejects
non-documentation drift between the two branches.

Licensed under the MIT License.
