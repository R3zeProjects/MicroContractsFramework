# Примеры использования

MCF содержит compile-time протоколы, а не runtime-реализации. Поэтому примеры
определяют собственные типы приложения и проверяют их через `static_assert`.

## Полное покрытие контрактов

Исполняемый [`examples/contracts.cpp`](../examples/contracts.cpp) охватывает:

| Контракт | Назначение |
|---|---|
| `Error<T>` | владеющий код и сообщение ошибки |
| `Result<R, E>` | expected-подобный канал результата |
| `ErrorModel<M>` | фабрика ошибок и семейство `Result<T>` |
| `LogEntry<T>` | структурированная запись |
| `LogSink<S, E>` | структурный `bool write(const E&)` sink |
| `TelemetryRecord<T>` | владеющая запись телеметрии |
| `TelemetryExporter<X, T>` | пакетный exporter |
| `ConfigurationSnapshot<T>` | неизменяемая ревизия конфигурации |
| `ConfigurationProvider<T>` | получение shared snapshot |
| `ConfigurationObserver<O, S>` | уведомление об изменении |

Минимальная модель ошибок находится в
[`examples/basic.cpp`](../examples/basic.cpp).

## Обобщённый алгоритм

```cpp
template<vosp::contracts::ErrorModel Model, typename Operation>
auto guarded(Operation&& operation)
{
    using Result = std::invoke_result_t<Operation&>;
    static_assert(vosp::contracts::Result<Result, typename Model::Error>);
    return std::invoke(operation);
}
```

Нарушение концепта должно оставаться ошибкой компиляции. Не добавляйте runtime
adapter: исправьте тип реализации. Негативные примеры находятся в
`tests/contracts_tests.cpp`.

Публичны только заголовки `include/vosp/contracts`. MCF не владеет потоками,
памятью runtime-компонентов, хранилищами или callback lifetime.
