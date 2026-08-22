# Architecture

## Scope

MCF owns only value-level contracts that may cross framework boundaries. It has
no runtime state, platform I/O, background workers, or dependency on another
VOSP framework.

```text
vosp::contracts <- MEF
       ^            ^
       |            |
    MPF core <- MPF-MEF adapter
```

## Invariants

- `Error` owns its diagnostic message.
- `Result<T>` is exactly `std::expected<T, Error>`.
- Public contracts use value semantics and contain no owning raw pointers.
- Dependency arrows may point to MCF; MCF never points back.
- Framework-specific error codes are owned by their framework, not MCF.

## Non-goals

MCF does not define persistence backends, logger sinks, register policies,
schedulers, or transport formats. Those concerns remain independently
replaceable.

