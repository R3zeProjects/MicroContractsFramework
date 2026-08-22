# API contracts

## Public surface

- `<vosp/contracts/error.hpp>`: `Error`, `Result`, and `ErrorModel` concepts;
- `<vosp/contracts/logging.hpp>`: `LogEntry` and `LogSink` concepts;
- `<vosp/contracts.hpp>`: supported umbrella;
- `vosp::contracts`: exported CMake target.

## Error model

An error is copyable and movable and exposes `code()` and `message()`. An error
model owns the actual `Error`, `Result<T>`, and `OperationResult` definitions and
provides `make_error`. MCF validates that API but never supplies its
implementation.

## Logging

A log entry exposes an error and level. A sink structurally provides
`bool write(const Entry&)`. Inheritance and a particular virtual interface are
not required by MCF.

## Diagnostics

Concept failures are compile-time API diagnostics. MCF performs no runtime
validation, allocation, exception translation, logging, or conversion.

## Stability

Breaking concept changes require a minor version increment before 1.0. New
optional concepts may be introduced in patch releases when existing satisfying
types remain valid.
