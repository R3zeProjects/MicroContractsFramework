# Примеры использования

MCF содержит протоколы compile-time, а не реализации runtime. Следовательно, примеры
определяют типы, принадлежащие приложению, и доказывают их совместимость с
`static_assert`.

## Полное покрытие по контракту

Компилируемый [`examples/contracts.cpp`](../examples/contracts.cpp) охватывает:

| Контракт | Требуемая роль |
|---|---|
| `Error<T>` | владение кодом и значением сообщения |
| `Result<R, E>` | expected-like канал успеха/ошибки |
| `ErrorModel<M>` | фабрика ошибок и семейство `Result<T>` |
| `LogEntry<T>` | структурированный уровень и значение ошибки |
| `LogSink<S, E>` | структурный `bool write(const E&)` sink |
| `TelemetryRecord<T>` | именной, с отметкой времени учетный запись |
| `TelemetryExporter<X, T>` | пакет exporter |
| `ConfigurationSnapshot<T>` | неизменяемая ревизия и поиск по ключу |
| `ConfigurationProvider<T>` | совместное приобретение snapshot |
| `ConfigurationObserver<O, S>` | явное уведомление об изменении |

Меньший [`examples/basic.cpp`](../examples/basic.cpp) демонстрирует минимальную
заменяемую модель ошибки.

## Генетический алгоритм

```cpp
template<vosp::contracts::ErrorModel Model, typename Operation>
auto guarded(Operation&& operation)
{
    using Result = std::invoke_result_t<Operation&>;
    static_assert(vosp::contracts::Result<Result, typename Model::Error>);
    return std::invoke(operation);
}
```

Сбой концепции преднамеренно является ошибкой compile-time. Не перехватывайте её и не
добавляйте адаптер runtime: измените тип реализации так, чтобы он соответствовал
протоколу. Отрицательные примеры находятся в `tests/contracts_tests.cpp`.

## Стабильная граница

Только заголовки под `include/vosp/contracts` являются общедоступными. MCF не владеет
выделение памяти, потоком, хранилищем, logger или exporter время жизни. Эти обязанности
остаются в реализации фреймворк или в корне композиции приложения.
