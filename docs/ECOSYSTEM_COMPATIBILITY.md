# Ecosystem compatibility

## Supported package set

The VOSP ecosystem is developed as independent repositories with one directional
dependency rule: runtime frameworks may depend on MCF contracts, while MCF never
depends on a runtime implementation. The following package set is validated as
one C++23 composition:

| Framework | Version line | CMake package | Imported target |
| --- | --- | --- | --- |
| MicroContractsFramework | 0.8.x beta | `vosp_contracts` | `vosp::contracts` |
| MicroProtocolFramework | 0.1.x beta | `mprotocol` | `vosp::protocol` |
| MicroErrorFramework | 0.6.x beta | `vosp` | `vosp::vosp` |
| MicroPersistenceFramework | 0.3.x beta | `mpf` | `vosp::persistence` |
| MicroTelemetryFramework | 0.1.1+ beta | `mtf` | `vosp::telemetry` |
| MicroConfigurationFramework | 0.1.x beta | `mconfig` | `vosp::configuration` |
| MicroResilienceFramework | 0.1.x beta | `mrf` | `vosp::resilience` |
| MicroWorkflowFramework | 0.1.x beta | `mwf` | `vosp::workflow` |
| MicroServiceFramework | 0.1.x beta | `msf` | `vosp::service` |

Patch releases preserve source compatibility. Before 1.0, a framework may make
a documented source-incompatible change in a minor release. Applications should
therefore constrain each package to the tested minor line instead of assuming
that every pre-1.0 combination is compatible.

## Ecosystem gate

MCF owns the cross-repository compatibility gate because it defines the shared
compile-time boundary without adding a runtime dependency. The gate:

1. checks out the current `main` branch of every runtime framework;
2. builds and installs each framework independently into one clean prefix;
3. disables dependency fetching so missing package metadata cannot be hidden;
4. configures a separate consumer exclusively through `find_package`;
5. rejects direct runtime-to-runtime includes in every core `include/` and `src/`
   tree and rejects runtime dependencies in the MCF build graph;
6. compiles and runs configuration, error, logging, protocol framing, persistence,
   telemetry, resilience, child-process workflow, and in-process service lifecycle
   operations together;
7. covers valid and invalid configuration, persistence rejection, retry recovery,
   direct export, bounded asynchronous drain, shutdown rejection, and non-zero
   child-process completion;
8. runs with GCC and Clang on Linux and MSVC on Windows.

The consumer lives in `tests/ecosystem_consumer`. The CI driver is
`.github/scripts/test-ecosystem-packages.ps1` and can be reproduced locally:

```powershell
./.github/scripts/test-ecosystem-packages.ps1 `
  -ContractsSource $PWD `
  -FrameworksRoot C:/src/vosp `
  -BuildRoot C:/temp/vosp-ecosystem `
  -CxxCompiler clang++
```

Dependency direction can be checked without building:

```powershell
./.github/scripts/test-ecosystem-boundaries.ps1 `
  -ContractsSource $PWD `
  -FrameworksRoot C:/src/vosp
```

`FrameworksRoot` must contain the eight runtime repository directories. The build
directory must be disposable; package installation and the downstream consumer
are generated below it.

## Failure interpretation

- Failure during a framework install indicates a package-local build or export
  defect.
- Failure while configuring the consumer indicates a missing or incompatible
  transitive CMake dependency.
- Failure while compiling the consumer indicates public-header or contract drift.
- Runtime failure indicates that independently packaged components compile but
  no longer compose correctly.
- Boundary-check failure indicates that a replaceable runtime component leaked
  into another framework's core or into MCF.

The gate proves compatibility of the tested package set; it does not promise ABI
stability across releases or replace each framework's unit, sanitizer, fuzz, and
stress suites.

External-project scope and benchmark interpretation are documented separately in
[the ecosystem comparison](ECOSYSTEM_COMPARISON.md).
