# Changelog

## Unreleased

- Added a Windows/Linux ecosystem gate that installs all eight frameworks as
  independent CMake packages and validates a separate downstream consumer.
- Added a dependency-direction check for all public and implementation source trees.
- Expanded the consumer with version contracts, asynchronous drain, retry,
  rejected persistence/configuration, and non-zero child-process scenarios.
- Documented the package compatibility matrix and failure-diagnosis policy.

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
