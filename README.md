# MicroContractsFramework

MicroContractsFramework (MCF) — header-only библиотека контрактов C++23 для
создания заменяемых компонентов экосистемы VOSP. Она задаёт требования к API на
этапе компиляции, но не реализует ошибки, результаты, логгеры, хранилища,
телеметрию, конфигурацию, планирование или ввод-вывод.

MEF предоставляет стандартную реализацию `Error`, `Result<T>` и логирования.
Остальные фреймворки могут использовать MEF или любой пользовательский тип,
который удовлетворяет тем же концептам MCF.

## Зачем нужен MCF

Фреймворки зависят от стабильных требований, а не от конкретных классов:

```text
                 MicroContractsFramework
                  концепты и требования
                    ^              ^
                    |              |
          реализация MEF      шаблоны MPF/MTF/...
                    \              /
                         приложение
```

Это compile-time dependency inversion без runtime-реестров, оболочек,
преобразований и адаптеров.

## Контракты

- `Error<T>` — владеющий тип ошибки;
- `Result<R, E>` — expected-подобный результат;
- `ErrorModel<M>` — модель ошибок и семейство `Result<T>`;
- `LogEntry<T>` и `LogSink<S, E>` — запись и структурный sink;
- `TelemetryRecord<T>` и `TelemetryExporter<X, T>` — телеметрия;
- `ConfigurationSnapshot<T>`, `ConfigurationProvider<T>` и
  `ConfigurationObserver<O, S>` — конфигурация.

MCF не выполняет работу во время исполнения: все проверки происходят при
компиляции. Полный исполняемый пример всех контрактов находится в
[`examples/contracts.cpp`](examples/contracts.cpp).

## Быстрый старт

```cpp
#include <vosp/contracts/error.hpp>
#include <expected>

struct MyError {
    std::uint32_t code() const noexcept;
    std::string_view message() const noexcept;
};

struct MyModel {
    using Error = MyError;
    template<class T> using Result = std::expected<T, Error>;
    using OperationResult = Result<void>;
    static Error make_error(std::uint32_t, std::string);
};

static_assert(vosp::contracts::ErrorModel<MyModel>);
```

## CMake

```cmake
find_package(vosp_contracts 0.6 REQUIRED CONFIG)
target_link_libraries(application PRIVATE vosp::contracts)
```

```sh
cmake -S . -B build -DBUILD_TESTING=ON -DMCF_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Подробности: [установка](docs/INSTALLATION.md),
[полное использование](docs/USAGE_EXAMPLES.md),
[архитектура](docs/ARCHITECTURE.md) и [API-контракты](docs/API_CONTRACTS.md).

MCF зависит только от C++23. Лицензия MIT.
