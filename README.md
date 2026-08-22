# MicroContractsFramework

MicroContractsFramework (MCF) is the dependency-neutral contract foundation for
the VOSP micro-framework ecosystem. It lets independently replaceable modules
exchange failures without depending on each other's runtime implementation.

MCF is header-only and provides:

- `vosp::error::Error`;
- `vosp::error::Result<T>` and `OperationResult` based on `std::expected`;
- shared operational categories;
- stable formatting, hashing, and exception translation;
- shared `LogEntry` and `ILogSink` contracts for direct logger/sink composition.

It deliberately does not contain logger implementations, persistence,
schedulers, registries, telemetry, I/O, or plugins.

## Usage

```cpp
#include <vosp/contracts/error.hpp>

vosp::error::Result<int> read_value(bool available)
{
    if (!available)
    {
        return std::unexpected(vosp::error::Error{
            vosp::error::Category::DATABASE, 1, "Value is unavailable"});
    }
    return 42;
}
```

## CMake

```cmake
find_package(vosp_contracts 0.2 REQUIRED CONFIG)
target_link_libraries(your_target PRIVATE vosp::contracts)
```

Build and test locally:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Requires C++23 and a standard library with `std::expected` and `std::format`.

## Dependency rule

Dependencies point toward contracts. MCF never depends on MEF, MPF, or any
future ecosystem framework. Cross-framework behavior belongs in optional edge
adapters, not in this package.

Licensed under the MIT License.
