# Средства тестирования

## Архитектурная граница

`vosp::contracts` остаётся production-зависимостью только для compile-time. Тестовые
цели могут подключить `vosp::testing`, чтобы получить детерминированные runner и
поддержку compile-fail в CMake. Ни один runner не запускает потоки и не выделяет
состояние до явного вызова приложением.

## Property-проверки

```cpp
const auto report = vosp::testing::check_property(
    {.cases = 100'000, .seed = 0xC0FFEE},
    [](vosp::testing::Random& random, std::size_t) {
        return random.uniform(0, 1'000);
    },
    [](std::uint64_t value) { return value <= 1'000; });

if (!report.passed()) {
    // Повторите запуск с тем же seed и проверьте report.first_failure.
}
```

`Random` использует стабильную последовательность SplitMix64 и rejection sampling для
несмещённой выборки из включительного диапазона. Результат property `false` или
исключение останавливает запуск на первом отказавшем случае.

## Stress и concurrency

`run_stress` повторно вызывает `bool operation(size_t iteration)` в текущем потоке.
`run_concurrently` вызывает один общий callable как
`bool operation(size_t worker, size_t iteration, std::stop_token)`.

```cpp
const auto report = vosp::testing::run_concurrently(
    {.workers = 8, .iterations_per_worker = 50'000, .stop_on_failure = true},
    [&](std::size_t worker, std::size_t iteration, std::stop_token stop) {
        return exercise_shared_component(worker, iteration, stop);
    });
```

Операция должна допускать конкурентный вызов. Worker ожидают общий старт, разделяют
отмену, сохраняют первое исключение и полностью присоединяются до возврата функции.
Количество worker ограничено диапазоном `[1, 1024]`; переполнение произведения
отклоняется до выделения памяти.

## Compile-fail контракты

Пакет устанавливает `VospTesting.cmake` и загружает его при `find_package`:

```cmake
include(CTest)
find_package(vosp_contracts 0.12 REQUIRED CONFIG)

vosp_add_compile_fail_test(
    NAME invalid_error_contract
    SOURCES invalid_error.cpp
    LINK_LIBRARIES vosp::contracts)
```

Исходник исключён из обычной сборки. CTest проходит только тогда, когда сборка
выделенной цели возвращает ошибку компилятора. Поэтому случайное ослабление концепта
приводит к красному CI.
