# Usage examples

MCF contains compile-time protocols, not runtime implementations. The examples
therefore define application-owned types and prove their compatibility with
`static_assert`.

## Complete contract coverage

The compilable [`examples/contracts.cpp`](../examples/contracts.cpp) covers:

| Contract | Required role |
|---|---|
| `Error<T>` | owning code and message value |
| `Result<R, E>` | expected-like success/error channel |
| `ErrorModel<M>` | error factory and `Result<T>` family |
| `LogEntry<T>` | structured level and error value |
| `LogSink<S, E>` | structural `bool write(const E&)` sink |
| `TelemetryRecord<T>` | named, timestamped owning record |
| `TelemetryExporter<X, T>` | batch exporter |
| `ConfigurationSnapshot<T>` | immutable revision and key lookup |
| `ConfigurationProvider<T>` | shared snapshot acquisition |
| `ConfigurationObserver<O, S>` | explicit change notification |

The smaller [`examples/basic.cpp`](../examples/basic.cpp) demonstrates the
minimum replaceable error model.

## Generic algorithm

```cpp
template<vosp::contracts::ErrorModel Model, typename Operation>
auto guarded(Operation&& operation)
{
    using Result = std::invoke_result_t<Operation&>;
    static_assert(vosp::contracts::Result<Result, typename Model::Error>);
    return std::invoke(operation);
}
```

Concept failure is intentionally a compile-time error. Do not catch it or add a
runtime adapter: change the implementation type so it satisfies the protocol.
Negative examples are maintained in `tests/contracts_tests.cpp`.

## Stable boundary

Only headers under `include/vosp/contracts` are public. MCF owns no allocation,
thread, storage, logger or exporter lifetime. Those responsibilities remain in
the implementation framework or application composition root.
