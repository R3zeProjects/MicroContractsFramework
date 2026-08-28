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
| `ProtocolMessage<T>` | immutable versioned message view |
| `ProtocolCodec<C, V, M>` | value-to-bytes codec |
| `ProtocolFramer<F, V, M>` | bounded message framing |
| `ProtocolStreamDecoder<D, V, M>` | incremental stream decoding |
| `ByteStreamTransport<C, M>` | ordered byte stream I/O |
| `TransportConnector<C, E, M>` | explicit connect and reconnect |
| `DatagramTransport<S, E, D, M>` | endpoint-aware datagram I/O |
| `PluginLifecycle<P, M>` | named start/stop plugin instance |
| `PluginFactory<F, M>` | owning plugin creation |
| `SecureBytes<B>` | owning bytes with explicit erasure |
| `DigestProvider<P, M>` | replaceable digest calculation |
| `MessageAuthenticator<A, M>` | keyed tag creation and verification |
| `PermissionPolicy<P, A, R>` | authorization decision for a resource |

The smaller [`examples/basic.cpp`](../examples/basic.cpp) demonstrates the
minimum replaceable error model.
[`examples/testing.cpp`](../examples/testing.cpp) demonstrates deterministic property
checks and bounded concurrent stress through the separate `vosp::testing` target.

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
Negative concept examples are maintained in `tests/contracts_tests.cpp`; actual expected
compiler failures are maintained under `tests/compile_fail`.

## Stable boundary

Headers under `include/vosp/contracts` and `include/vosp/testing` are public. The core
contract target owns no allocation, thread, storage, logger or exporter lifetime. Those
responsibilities remain in the implementation framework or application composition root.
Test threads exist only during an explicit `run_concurrently` call and are joined before
it returns.
