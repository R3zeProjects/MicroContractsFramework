# Architecture

## Scope

MCF owns compile-time protocols shared across independently replaceable
frameworks. It contains concepts and version metadata, but no concrete domain
implementation, runtime state, allocation policy, I/O, worker, or global
registry.

The exported `vosp::testing` companion target is deliberately outside that core
boundary. It contains opt-in test runners that allocate threads only when a test
explicitly invokes `run_concurrently`. Linking only `vosp::contracts` preserves the
zero-runtime contract model.

## Operating principles

MCF applies **structural contracts at compile time**. A consumer passes a type
to a framework template; the selected concept checks the required expressions,
their constness, and their exact result semantics. A matching type is composed
directly into the algorithm. A mismatch stops compilation at the boundary
instead of becoming a runtime branch or failed cast.

```text
candidate type -> concept substitution -> required operations checked
                                      -> accepted: direct static composition
                                      -> rejected: compile-time diagnostic
```

The contracts describe capability, not ancestry. Implementations do not derive
from MCF classes and MCF does not create, own, allocate, schedule, or destroy
their objects. Including MCF therefore adds no runtime state and dependency
arrows always point from concrete frameworks to contracts. Exact expected-like
result requirements keep failures explicit while allowing the concrete error
representation to be replaced.

## Dependency direction

```text
                 vosp::contracts
                 concepts only
                   ^        ^
                   |        |
                  MEF      MPF
                   \        /
                    program
```

MEF implements the standard error and logging model. MPF is parameterized by an
error model and may use MEF's implementation or another compatible model.

## Invariants

- MCF concepts use structural compile-time checks.
- Satisfying a contract never requires inheritance unless a future contract
  explicitly documents it.
- MCF creates no framework value and owns no runtime resource.
- `vosp::testing` is test-only, bounded, joined, and never activated implicitly.
- MCF never includes MEF or MPF headers.
- Implementations are rejected at compile time when required operations or
  result semantics are missing.

## Non-goals

MCF does not provide a default error, logger, sink, backend, codec, scheduler,
formatter, hashing algorithm, exception translator, persistence format, or unit-test
assertion framework.
