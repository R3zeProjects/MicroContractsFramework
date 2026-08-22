# API contracts

## Includes and target

- Canonical error header: `<vosp/contracts/error.hpp>`.
- Umbrella header: `<vosp/contracts.hpp>`.
- Exported CMake target: `vosp::contracts`.

## Ownership and lifetime

`Error` owns its message. Values returned from `message()` are views into that
owned string and remain valid until the `Error` is mutated, moved from, or
destroyed.

## Failures

`Result<T>` is `std::expected<T, Error>`. Recoverable domain failures cross
framework boundaries as values; MCF itself performs no logging or persistence.

`attempt` catches exceptions thrown by the supplied operation. It appends a
`std::exception` diagnostic to the fallback message and preserves the fallback
error for unknown exceptions. Allocation failures while constructing the
diagnostic may propagate.

## Stability

The `vosp::error` namespace is shared intentionally. Framework-specific numeric
codes remain owned and documented by the framework that emits them.
