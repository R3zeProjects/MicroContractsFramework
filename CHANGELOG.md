# Changelog

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
