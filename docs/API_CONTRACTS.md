# API contracts

## Public surface

- `<vosp/contracts/error.hpp>`: `Error`, `Result`, and `ErrorModel` concepts;
- `<vosp/contracts/cache.hpp>`: owning key-value cache and expiration cleanup;
- `<vosp/contracts/logging.hpp>`: `LogEntry` and `LogSink` concepts;
- `<vosp/contracts/plugin.hpp>`: named plugin lifecycle and owning factory concepts;
- `<vosp/contracts/protocol.hpp>`: message, codec, framing, and stream-decoder concepts;
- `<vosp/contracts/transport.hpp>`: byte-stream, connector, and datagram concepts;
- `<vosp/contracts/security.hpp>`: secure-byte, digest, authenticator, and permission concepts;
- `<vosp/contracts/configuration.hpp>`: snapshot, provider, and observer concepts;
- `<vosp/contracts/telemetry.hpp>`: record and exporter concepts;
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

## Security providers

`SecureBytes` requires mutable and immutable byte spans plus a `noexcept`
explicit erasure operation. `DigestProvider` and `MessageAuthenticator` define
result-bearing cryptographic provider shapes but never select or implement an
algorithm. `PermissionPolicy` describes only an authorization decision; concrete
permission and resource types remain application-owned.

## Plugins

`PluginLifecycle<Type, Model>` requires stable name and version views plus explicit
`start()` and `stop()` operations using the selected error model.
`PluginFactory<Factory, Model>` obtains `Plugin` from `Factory::plugin_type` and
requires an owning `Result<std::unique_ptr<Plugin>>`. MCF does not define a base class, registry,
dynamic-library loader, ABI structure, allocation mechanism, or unload policy.

## Diagnostics

Concept failures are compile-time API diagnostics. MCF performs no runtime
validation, allocation, exception translation, logging, or conversion.

## Stability

Breaking concept changes require a minor version increment before 1.0. New
optional concepts may be introduced in patch releases when existing satisfying
types remain valid.
