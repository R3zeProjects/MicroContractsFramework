# MicroContractsFramework

MicroContractsFramework (MCF) — header-only библиотека контрактов C++23 для создания
заменяемых компонентов экосистемы VOSP. Она определяет требования API на этапе
компиляции, но не реализует ошибки, результаты, реестры, sinks, persistence, хранение
конфигурации, планирование или ввод/вывод.

MEF предоставляет стандартную production-реализацию типов `Error`, `Result<T>` и
логгирования. MPF и будущие фреймворки могут принимать эту реализацию или любую
пользовательскую реализацию, удовлетворяющую тем же концепциям MCF.

## Почему существует MCF

Фреймворки зависят от стабильных требований, а не от конкретных классов друг друга:

```text
                 MicroContractsFramework
                 concepts / requirements
                    ^              ^
                    |              |
          MEF implementation    MPF templates
                    \              /
                     application
```

Это обеспечивает compile-time инверсию зависимостей без runtime-реестров,
объектов-обёрток, преобразований или слоёв адаптера.

## Контракты

- `vosp::contracts::Error<T>` проверяет API с ошибкой владения;
- `vosp::contracts::Result<R, E>` проверяет API результата expected-like;
- `vosp::contracts::ErrorModel<M>` проверяет поставщика ошибок/результатов;
- `vosp::contracts::LogEntry<T>` проверяет структурированную запись;
- `vosp::contracts::LogSink<S, E>` проверяет структурную реализацию sink.
- `vosp::contracts::TelemetryRecord<T>` проверяет значение принадлежащей телеметрии;
- `vosp::contracts::TelemetryExporter<X, T>` проверяет партию exporter.
- `vosp::contracts::ConfigurationSnapshot<T>` проверяет неизменяемый snapshot;
- `vosp::contracts::ConfigurationProvider<T>` проверяет публикацию snapshot;
- `vosp::contracts::ConfigurationObserver<O, S>` проверяет наблюдателей изменений.
- `vosp::contracts::ProtocolMessage<T>` проверяет неизменяемые представления сообщений;
- `vosp::contracts::ProtocolCodec<C, V, M>` проверяет кодеки значений;
- `vosp::contracts::ProtocolFramer<F, V, M>` проверяет фрейминг сообщений;
- `vosp::contracts::ProtocolStreamDecoder<D, V, M>` проверяет инкрементальное
  декодирование потока.

MCF не содержит конкретного класса `Error`, `Result`, `LogEntry` или `Sink`. Его
контракт выполнения — нулевая работа runtime: все проверки являются концепциями,
оцениваемыми на этапе компиляции. Отрицательные тесты теперь также отвергают sinks и
телеметрию exporters, чьи возвращаемые типы не соответствуют общим контрактам.

## Пользовательская реализация

```cpp
#include <vosp/contracts/error.hpp>

#include <expected>
#include <string>
#include <string_view>

struct MyError
{
    std::uint32_t code() const noexcept;
    std::string_view message() const noexcept;
};

struct MyErrorModel
{
    using Error = MyError;

    template<class T>
    using Result = std::expected<T, Error>;

    using OperationResult = Result<void>;

    static Error make_error(std::uint32_t code, std::string message);
};

static_assert(vosp::contracts::ErrorModel<MyErrorModel>);
```

Реализация MEF удовлетворяет тому же контракту, поэтому фреймворк, параметризованный
`ErrorModel`, может менять реализации без изменения своих алгоритмов.

## CMake

Смотрите [installation guide](docs/INSTALLATION.md) для рабочих процессов дерева
исходного кода и установленных пакетов. Компилируемые примеры и шаблоны композиции
экосистемы собраны в [usage examples](docs/USAGE_EXAMPLES.md). [architecture
note](docs/ARCHITECTURE.md) объясняет замену контрактов, направление зависимостей и
операционную модель нулевого runtime.
[Матрица совместимости экосистемы](docs/ECOSYSTEM_COMPATIBILITY.md) фиксирует
совместно проверяемые линии пакетов и описывает integration gate для Windows/Linux.
[Сравнение экосистемы](docs/ECOSYSTEM_COMPARISON.md) отделяет воспроизводимые
benchmark одинаковой работы от функционального сравнения с известными проектами.

```cmake
find_package(vosp_contracts 0.7 REQUIRED CONFIG)
target_link_libraries(your_target PRIVATE vosp::contracts)
```

Собрать и протестировать:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Поддерживаемые CI-инструментальные цепочки: GCC, Clang и MSVC. MCF требует C++23 и не
имеет зависимости от runtime или сторонних компонентов.

## Правило зависимости

Зависимости могут указывать на MCF. MCF никогда не зависит от MEF, MPF или другой
реализации экосистемы. Конкретное поведение остается в фреймворк, который им владеет,
или в корневой композиции конечного приложения.

## Проверка экосистемы и языковые ветки

MCF владеет gate совместимости экосистемы, но не runtime-композицией. Gate независимо
устанавливает все девять пакетов, запрещает недопустимые зависимости в core и запускает
успешные и отказные сценарии, retry, асинхронный drain и дочерние процессы через
downstream consumer на основе `find_package`. См.
[матрицу совместимости](docs/ECOSYSTEM_COMPATIBILITY.md).

`main` содержит канонический исходный код и английскую документацию. `main-ru`
содержит тот же исходный код и русскую Markdown-документацию. CI отклоняет любые
различия между ветками за пределами Markdown-файлов.

Лицензировано по лицензии MIT.
