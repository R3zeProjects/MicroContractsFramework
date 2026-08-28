# Changelog

## Unreleased

- Added a Windows/Linux ecosystem gate that installs all eight frameworks as
  independent CMake packages and validates a separate downstream consumer.
- Added a dependency-direction check for all public and implementation source trees.
- Expanded the consumer with version contracts, asynchronous drain, retry,
  rejected persistence/configuration, and non-zero child-process scenarios.
- Updated the package consumer to require MPF 0.4 and exercise its bounded cache
  through the shared `KeyValueCache` contract.
- Documented the package compatibility matrix and failure-diagnosis policy.

## 0.11.0-beta

- Added implementation-free `PluginLifecycle` and `PluginFactory` contracts.
- Defined named lifecycle operations and owning factory results without providing
  a registry, loader, ABI, or concrete plugin base class.
- Added standalone header, package-consumer, positive, and negative contract coverage.

## 0.10.0-beta

- Added the implementation-free `KeyValueCache` structural contract.
- Defined owning lookup, replacement, capacity, erasure, and explicit
  expiration-cleanup signatures without imposing an eviction implementation.
- Updated the ecosystem gate for MicroProtocolFramework 0.3 security support.

## 0.9.0-beta

- Added structural contracts for secure byte ownership and explicit erasure.
- Added replaceable digest and keyed message-authenticator provider contracts.
- Added a generic permission-policy contract without owning an authorization model.

## 0.8.0-beta

- Added structural contracts for ordered byte streams and reconnectable connectors.
- Added owning datagram and endpoint-aware datagram transport contracts.
- Kept sockets, polling, DNS, and platform handles outside MCF.
- Extended the ecosystem package gate to consume the compiled `vosp::transport` target.

## 0.7.0-beta

- Added structural contracts for versioned protocol messages and value codecs.
- Added replaceable framing and incremental stream-decoder contracts.
- Kept sockets, reconnect policy, cryptography, compression, and plugin loading
  outside MCF.

## 0.6.0-beta

- Added structural configuration snapshot, provider, and observer contracts.
- Kept configuration values, parsing, storage, and reload policy outside MCF.

## 0.5.0-beta

- Added structural `TelemetryRecord` and `TelemetryExporter` contracts.
- Kept telemetry storage, instrumentation, scheduling, and transport outside MCF.

## 0.4.0-beta

- Replaced concrete ecosystem value types with prototype-only C++ concepts.
- Added `Error`, `Result`, `ErrorModel`, `LogEntry`, and structural `LogSink`
  requirements for arbitrary compatible implementations.
- Added self-contained header and negative concept checks.

## 0.3.0-beta

- Restricted MCF to value schemas, aliases, concepts, and pure interfaces.
- Moved formatting, hashing, and exception-translation behavior back to MEF.

## 0.2.0-beta

- Added direct `Level`, `LogEntry`, `ILogSink`, and `SinkType` contracts.
- Enabled independent frameworks to produce and consume logs without depending
  on a concrete logger implementation or an integration adapter.

## 0.1.0-beta

- Added the dependency-neutral `vosp::error` value contracts.
- Added header-only CMake package target `vosp::contracts`.
- Added Windows, GCC, Clang, unit, and installed-package CI contracts.
