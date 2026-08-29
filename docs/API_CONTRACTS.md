# Контракты API

## Общественная поверхность

- `<vosp/contracts/error.hpp>`: концепты `Error`, `Result` и `ErrorModel`;
- `<vosp/contracts/cache.hpp>`: владеющий key-value cache и очистка expiration;
- `<vosp/contracts/logging.hpp>`: концепты `LogEntry` и `LogSink`;
- `<vosp/contracts/plugin.hpp>`: концепты именованного жизненного цикла плагина и
  владеющей фабрики;
- `<vosp/contracts/protocol.hpp>`: сообщения, кодеки, framing и stream decoder;
- `<vosp/contracts/transport.hpp>`: byte-stream, connector и datagram;
- `<vosp/contracts/security.hpp>`: secure-byte, digest, authenticator и permission;
- `<vosp/contracts/configuration.hpp>`: snapshot, provider и observer;
- `<vosp/contracts/telemetry.hpp>`: record и exporter;
- `<vosp/contracts.hpp>`: поддерживаемый umbrella;
- `vosp::contracts`: экспортированная цель CMake.

Сопутствующая поверхность тестирования намеренно отделена:

- `<vosp/testing.hpp>`: umbrella для property, stress, concurrency, random и report;
- `vosp::testing`: экспортированная header-only цель CMake;
- `VospTesting.cmake`: установленная регистрация compile-fail тестов CTest.

## Модель Error

Ошибка является копируемой и перемещаемой и предоставляет `code()` и `message()`. Модель
ошибки владеет фактическими определениями `Error`, `Result<T>` и `OperationResult` и
предоставляет `make_error`. MCF проверяет этот API, но никогда не предоставляет его
реализацию.

## Ведение журнала

Запись журнала предоставляет ошибку и уровень. Sink структурно реализует
`bool write(const Entry&)`. Наследование и конкретный виртуальный интерфейс не требуются
MCF.

## Security-provider

`SecureBytes` требует изменяемый и неизменяемый span байтов и явную операцию
стирания `noexcept`. `DigestProvider` и `MessageAuthenticator` определяют
форму provider с result, но не выбирают и не реализуют алгоритм.
`PermissionPolicy` описывает только решение авторизации; типы permission и
resource принадлежат приложению.

## Плагины

`PluginLifecycle<Type, Model>` требует стабильные представления имени и версии, а
также явные операции `start()` и `stop()` с выбранной моделью ошибок.
`PluginFactory<Factory, Model>` получает тип плагина из `Factory::plugin_type` и
требует владеющий `Result<std::unique_ptr<Plugin>>`. MCF не определяет базовый класс,
реестр, загрузчик динамических библиотек, структуру ABI, механизм выделения памяти
или политику выгрузки.

## Диагностика

Сбой концепции является диагностикой API compile-time. MCF не выполняет проверку
runtime, выделение памяти, трансляцию исключений, логгирование или конвертацию.
Необязательная поверхность тестирования перехватывает исключения тестовой операции в
`TestReport`, но не изменяет семантику контрактов.

## Стабильность

Изменения концепции, нарушающие совместимость, требуют увеличения минорной версии перед
1.0.. Новые необязательные концепции могут быть введены в патч-релизах, если
существующие удовлетворяющие типы остаются допустимыми.
