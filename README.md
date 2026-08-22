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

MCF contains no concrete `Error`, `Result`, `LogEntry`, or `Sink` class.

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

```cmake
find_package(vosp_contracts 0.6 REQUIRED CONFIG)
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

Licensed under the MIT License.
