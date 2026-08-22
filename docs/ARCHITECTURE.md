# Architecture

## Scope

MCF owns compile-time protocols shared across independently replaceable
frameworks. It contains concepts and version metadata, but no concrete domain
implementation, runtime state, allocation policy, I/O, worker, or global
registry.

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
- MCF never includes MEF or MPF headers.
- Implementations are rejected at compile time when required operations or
  result semantics are missing.

## Non-goals

MCF does not provide a default error, logger, sink, backend, codec, scheduler,
formatter, hashing algorithm, exception translator, or persistence format.
