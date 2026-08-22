# API contracts

## Includes and target

- Canonical error header: `<vosp/contracts/error.hpp>`.
- Canonical logging boundary: `<vosp/contracts/logging.hpp>`.
- Umbrella header: `<vosp/contracts.hpp>`.
- Exported CMake target: `vosp::contracts`.

## Ownership and lifetime

`Error` owns its message. Values returned from `message()` are views into that
owned string and remain valid until the `Error` is mutated, moved from, or
destroyed.

## Failures

`Result<T>` is `std::expected<T, Error>`. Recoverable domain failures cross
framework boundaries as values; MCF itself performs no logging or persistence.

`LogEntry` owns its `Error`. `ILogSink::write` receives a const reference valid
only for the duration of the call. Implementations that retain an entry must
copy or move an owning value into their own storage.

MCF contains no formatting, hashing, exception translation, logging, or
persistence algorithms. Those implementations belong to MEF, MPF, or another
framework that consumes these contracts.

## Stability

The `vosp::error` namespace is shared intentionally. Framework-specific numeric
codes remain owned and documented by the framework that emits them.
