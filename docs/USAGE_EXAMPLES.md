# Примеры использования

MCF содержит протоколы compile-time, а не реализации runtime. Следовательно, примеры
определяют типы, принадлежащие приложению, и доказывают их совместимость с
`static_assert`.

## Полное покрытие по контракту

Компилируемый [`examples/contracts.cpp`](../examples/contracts.cpp) охватывает:

| Контракт | Требуемая роль |
|---|---|
| `Error<T>` | Владение кодом и значением сообщения |
| `Result<R, E>` | expected-like канал успеха/ошибки |
| `ErrorModel<M>` | Фабрика ошибок и семейство `Result<T>` |
| `LogEntry<T>` | Структурированный уровень и значение ошибки |
| `LogSink<S, E>` | Структурный `bool write(const E&)` sink |
| `TelemetryRecord<T>` | Именной, с отметкой времени учетный запись |
| `TelemetryExporter<X, T>` | Пакет exporter |
| `ConfigurationSnapshot<T>` | Неизменяемая ревизия и поиск по ключу |
| `ConfigurationProvider<T>` | Совместное приобретение snapshot |
| `ConfigurationObserver<O, S>` | Явное уведомление об изменении |
| `ProtocolMessage<T>` | Неизменяемое представление версионированного сообщения |
| `ProtocolCodec<C, V, M>` | Кодек значения в байты |
| `ProtocolFramer<F, V, M>` | Ограниченный фрейминг сообщений |
| `ProtocolStreamDecoder<D, V, M>` | Инкрементальное декодирование потока |
| `ByteStreamTransport<C, M>` | Упорядоченный ввод-вывод байтов |
| `TransportConnector<C, E, M>` | Явные connect и reconnect |
| `DatagramTransport<S, E, D, M>` | Ввод-вывод датаграмм с endpoint |
| `PluginLifecycle<P, M>` | Именованный экземпляр плагина с запуском и остановкой |
| `PluginFactory<F, M>` | Владеющее создание плагина |
| `SecureBytes<B>` | Владеющие байты с явным стиранием |
| `DigestProvider<P, M>` | Заменяемое вычисление digest |
| `MessageAuthenticator<A, M>` | Создание и проверка keyed-tag |
| `PermissionPolicy<P, A, R>` | Решение авторизации для resource |

Меньший [`examples/basic.cpp`](../examples/basic.cpp) демонстрирует минимальную
заменяемую модель ошибки.
[`examples/testing.cpp`](../examples/testing.cpp) демонстрирует детерминированные
property-проверки и ограниченный concurrent stress через отдельную цель `vosp::testing`.

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
протоколу. Отрицательные примеры концептов находятся в `tests/contracts_tests.cpp`, а
реальные ожидаемые отказы компилятора — в `tests/compile_fail`.

## Стабильная граница

Заголовки в `include/vosp/contracts` и `include/vosp/testing` являются публичными.
Цель core-контрактов не владеет выделением памяти, потоком, хранилищем и временем жизни
logger или exporter. Эти обязанности остаются в реализации фреймворка или в корне
композиции приложения. Тестовые потоки существуют только во время явного вызова
`run_concurrently` и присоединяются до его возврата.
